// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
ASnakePawn::ASnakePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = CollisionComponent;

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 800.0f;
	SpringArm->bInheritPitch = false; // Keep camera top-down-ish

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	CurrentGridDir = FVector(1, 0, 0); // Start moving Right
	NextGridDir = CurrentGridDir;
	MoveTimer = 0.0f;

}

// Called when the game starts or when spawned
void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();
	
	// Initial Position (Start with head and one tail segment)
	SegmentLocations.Add(GetActorLocation());
	AddSegment(); 

	if (APlayerController* PlayerCont = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerCont->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SnakeMappingContext, 0);
		}
	}
}

// Called every frame
void ASnakePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveTimer += DeltaTime;

	if (MoveTimer >= MoveInterval)
	{
		PerformGridStep();
		MoveTimer = 0.0f;
	}

	UpdateVisuals(DeltaTime);
}

// Called to bind functionality to input
void ASnakePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASnakePawn::HandleMove);
	}
}

void ASnakePawn::HandleMove(const FInputActionValue& Value)
{
	// Check if in a tunnel or on a bridge
	if (bIsMovementLocked) return;

	FVector2D Input = Value.Get<FVector2D>();
	FVector NewDir = FVector::ZeroVector;

	if (FMath::Abs(Input.X) > 0.5f) NewDir = FVector(Input.X > 0 ? 1 : -1, 0, 0);
	else if (FMath::Abs(Input.Y) > 0.5f) NewDir = FVector(0, Input.Y > 0 ? 1 : -1, 0);

	// Prevent 180-degree turns
	if (NewDir != FVector::ZeroVector && NewDir != -CurrentGridDir)
	{
		NextGridDir = NewDir;
	}
}

void ASnakePawn::PerformGridStep()
{
	// check current cell
	int32 CurrentIdx = GridGen->GetIndex(CurrentGridX, CurrentGridY, CurrentLayer);
    FGridCell& CurrentCell = GridGen->GridData[CurrentIdx];

    if (CurrentCell.bIsForcedPath)
    {
        bIsMovementLocked = true;
        NextGridDir = CurrentCell.ForcedDir; // Override player choice
    }
    else
    {
        bIsMovementLocked = false;
    }

	CurrentGridDir = NextGridDir;

	// Calculate new head position
	FVector NewHeadLoc = SegmentLocations[0] + (CurrentGridDir * TileSize);

	// Check the cell player is moving into
    int32 TargetX = FMath::RoundToInt(NewHeadGridLoc.X / TileSize);
    int32 TargetY = FMath::RoundToInt(NewHeadGridLoc.Y / TileSize);
    
    // Check all layers at that coordinate to see where player ends up
    // If on Layer 0 and hit a RampUp, change CurrentLayer to 1, then eventually 2.
    CheckLayerTransition(TargetX, TargetY);

	// Shift all segments forward
	for (int32 i = SegmentLocations.Num() - 1; i > 0; i--)
	{
		SegmentLocations[i] = SegmentLocations[i - 1];
	}
	SegmentLocations[0] = NewHeadLoc;

	// Rotate head to face direction
	SetActorRotation(CurrentGridDir.Rotation());
}

void ASnakePawn::UpdateVisuals(float DeltaTime)
{
	// Smoothly interpolate the Head Actor to its logical target
	// The visual location is a Lerp between where we were and where we are going
	float Alpha = MoveTimer / MoveInterval;

	// Update Head
	SetActorLocation(FMath::Lerp(SegmentLocations[0] - (CurrentGridDir * TileSize), SegmentLocations[0], Alpha));

	// Update Body Parts
	for (int32 i = 0; i < BodyParts.Num(); i++)
	{
		// Each body part 'i' follows segment 'i+1'
		FVector CurrentPos = SegmentLocations[i+1];
		FVector PrevPos = (i == 0) ? SegmentLocations[0] : SegmentLocations[i];
        
		FVector VisualPos = FMath::Lerp(CurrentPos, PrevPos, Alpha);
		BodyParts[i]->SetWorldLocation(VisualPos);
        
		// Face the direction of the segment it's following
		FVector Dir = (PrevPos - CurrentPos).GetSafeNormal();
		if(!Dir.IsNearlyZero()) BodyParts[i]->SetWorldRotation(Dir.Rotation());
	}
}

void ASnakePawn::AddSegment()
{
	UStaticMeshComponent* NewPart = NewObject<UStaticMeshComponent>(this);
	NewPart->RegisterComponent();
	NewPart->SetStaticMesh(BodyMeshAsset);
	NewPart->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Only head handles collisions
    
	// Logic: Add a location at the very end
	FVector LastLoc = SegmentLocations.Last();
	SegmentLocations.Add(LastLoc);
    
	BodyParts.Add(NewPart);
}