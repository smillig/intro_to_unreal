// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePawn.h"
#include "AGridGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
	PrimaryActorTick.TickGroup = TG_PostPhysics;
	
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = CollisionComponent;

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 800.0f;
	SpringArm->bInheritPitch = false; // Keep camera top-down-ish
	SpringArm->bUsePawnControlRotation = false; // fixed camera, not controller-driven
	SpringArm->SetUsingAbsoluteRotation(true);  // fixed world rotation, not relative to pawn rotation
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	MoveTimer = 0.0f;

}

// Called when the game starts or when spawned
void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();
	// debugging
	// if (GridGen) {
	// 	UE_LOG(LogTemp, Warning, TEXT("Grid initialized with %d cells"), GridGen->GridData.Num());
	// }
	// Find the Grid Generator in the level
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AAGridGenerator::StaticClass());
	GridGen = Cast<AAGridGenerator>(FoundActor);
	
	FVector SpawnLocation = GetActorLocation();

	// Initialize logic positions based on spawn
	CurrentGridLocation.X = FMath::RoundToInt(SpawnLocation.X / TileSize);
	CurrentGridLocation.Y = FMath::RoundToInt(SpawnLocation.Y / TileSize);
	CurrentLayer = 1;
	
	// get the grids z height for cell
	int32 Idx = GridGen->GetIndex(CurrentGridLocation.X, CurrentGridLocation.Y, CurrentLayer);
	float GridBaseZ = 0.0f;
	if (GridGen->GridData.IsValidIndex(Idx))
	{
		GridBaseZ = GridGen->GridData[Idx].ZOffset;
	}
	
	// snap pawn to location
	FVector SnappedLocation = FVector(CurrentGridLocation.X * TileSize, CurrentGridLocation.Y * TileSize, GridBaseZ + VerticalOffset);
		
	SetActorLocation(SnappedLocation);
	StepStartWorldLocation = SnappedLocation;
	StepTargetWorldLocation = SnappedLocation; 
	SegmentLocations.Add(SnappedLocation);
	
	// start with just head
	SegmentLocations.Empty();
	SegmentLocations.Add(SnappedLocation);

	if (APlayerController* PlayerCont = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerCont->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SnakeMappingContext, 0);
		}
	}
	
	// get history started
	int32 InitialHistory = (BodyParts.Num() + 1) * PointsPerSegment;
	for (int32 i = 0; i < InitialHistory; i++)
	{
		SnakeHistory.Add(SpawnLocation);
	}
	
	// player starts moving immediatly
	bIsMovingToTarget = false;
	MoveTimer = MoveInterval;
}

// Called every frame
void ASnakePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead || !GridGen) return;

	// Run the movement logic every frame for smooth interpolation
	GridMove(DeltaTime);
	
	// breadcrumbs for snake history
	FVector CurrentLocation = GetActorLocation();
	
	if (SnakeHistory.Num() == 0 || FVector::Dist(CurrentLocation, SnakeHistory[0]) >= HistoryDistanceThreshold)
	{
		SnakeHistory.Insert(CurrentLocation, 0);
		// trim history
		int32 MaxHistoryNeeded = (BodyParts.Num() + 1) * PointsPerSegment;
		if (SnakeHistory.Num() > MaxHistoryNeeded + 2)
		{
			SnakeHistory.Pop();
		}
	}
	
	UpdateBodyVisuals();
    
	if (bIsDrawDebugInfo) DrawDebugInfo();

}

void ASnakePawn::GridMove(float DeltaTime)
{
	// Check if in a tunnel or on a bridge or if in move interval time
	if (bIsMovementLocked || (MoveInterval <= 0.f)) return;

	if (!bIsMovingToTarget)
	{
		// If at target, check for direction change and update target location
		HandleDirectionChange();

		const FIntPoint GridOffset = DirectionToGridOffset(CurrentDirection);
		PendingNextGridLocation = CurrentGridLocation + GridOffset;

		CheckLayerTransition(PendingNextGridLocation.X, PendingNextGridLocation.Y);
		
		if (WouldHitWall(PendingNextGridLocation))
		{
			HandleSnakeDeath();
			return;
		}

		StepStartWorldLocation = GetActorLocation();
		StepTargetWorldLocation = GridToWorldLocation(PendingNextGridLocation);
		// Since we're just starting to move towards the new target, we reset the interpolation progress to 0
		MoveInterpolationProgress = 0.f; 
		bIsMovingToTarget = true;
	}

	MoveInterpolationProgress += DeltaTime / MoveInterval;
	const float Alpha = FMath::Clamp(MoveInterpolationProgress, 0.f, 1.f);

	// Use Lerp for constant speed across the cell
	const FVector NewLocation = FMath::Lerp(StepStartWorldLocation, StepTargetWorldLocation, Alpha);
	SetActorLocation(NewLocation, false);

	if (Alpha >= 1.f)
	{
		// Update Segment logic positions
		for (int32 i = SegmentLocations.Num() - 1; i > 0; i--) {
			SegmentLocations[i] = SegmentLocations[i - 1];
		}
		SegmentLocations[0] = StepStartWorldLocation; // Where the head just came from

		// Update the actual Mesh components to match SegmentLocations
		// for (int32 i = 0; i < BodyParts.Num(); i++) {
		// 	BodyParts[i]->SetWorldLocation(SegmentLocations[i]);
		// 	if (i >= 1)
		// 	{
		// 		if (SegmentLocations[i-1].X != SegmentLocations[i].X)
		// 		{
		// 			BodyParts[i]->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
		// 		}
		// 		else if (SegmentLocations[i-1].Y != SegmentLocations[i].Y)
		// 		{
		// 			BodyParts[i]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		// 		}
		// 	}
		// 	else
		// 	{
		// 		if (SegmentLocations[i+1].X != SegmentLocations[i].X)
		// 		{
		// 			BodyParts[i]->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
		// 		}
		// 		else if (SegmentLocations[i+1].Y != SegmentLocations[i].Y)
		// 		{
		// 			BodyParts[i]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		// 		}
		// 	}
		// }
		// We've reached the target grid location
		CurrentGridLocation = PendingNextGridLocation;
		
		bIsMovingToTarget = false; // We're now at the target, so we can start the process again on the next tick
	}
}

void ASnakePawn::CheckLayerTransition(int32 TargetX, int32 TargetY)
{
	if (!GridGen) return;

	// Check all layers at the target X,Y to see where we should be
	for (int32 z = 0; z < GridGen->TotalLayers; z++)
	{
		int32 Idx = GridGen->GetIndex(TargetX, TargetY, z);
		if (GridGen->GridData.IsValidIndex(Idx))
		{
			ECellType T = GridGen->GridData[Idx].CellType;
            
			// If we are on the ground and hit a RampUp, move to Mid Layer
			if (CurrentLayer == 0 && T == ECellType::RampUp) { CurrentLayer = 1; break; }
            
			// If we are on Mid and hit Elevated, move to Top Layer
			if (CurrentLayer == 1 && T == ECellType::Elevated) { CurrentLayer = 2; break; }
            
			// If we are on Mid and hit RampDown, move to Ground
			if (CurrentLayer == 1 && T == ECellType::RampDown) { CurrentLayer = 0; break; }
		}
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

void ASnakePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// We check:
	// 1. That the PlayerInputComponent is a UEnhancedInputComponent, which it should be if we're using the Enhanced Input system; and
	// 2. That the MoveAction and TurnAction are set, to avoid binding to null actions
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (TurnUpAction)
		{
			EnhancedInputComponent->BindAction(TurnUpAction, ETriggerEvent::Triggered, this, &ASnakePawn::Input_TryTurnUp);
		}
		if (TurnDownAction)
		{
			EnhancedInputComponent->BindAction(TurnDownAction, ETriggerEvent::Triggered, this, &ASnakePawn::Input_TryTurnDown);
		}

		if (TurnLeftAction)
		{
			EnhancedInputComponent->BindAction(TurnLeftAction, ETriggerEvent::Triggered, this, &ASnakePawn::Input_TryTurnLeft);
		}
		if (TurnRightAction)
		{
			EnhancedInputComponent->BindAction(TurnRightAction, ETriggerEvent::Triggered, this, &ASnakePawn::Input_TryTurnRight);
		}
	}

}

void ASnakePawn::Input_TryTurnUp(const FInputActionValue& Value)
{
	bool bPressed = Value.Get<bool>();
	if (bPressed)
	{

		RequestedDirection = ESnakeDirection::Up;
	}
}

void ASnakePawn::Input_TryTurnDown(const FInputActionValue& Value)
{
	bool bPressed = Value.Get<bool>();
	if (bPressed)
	{
		RequestedDirection = ESnakeDirection::Down;
	}
}

void ASnakePawn::Input_TryTurnLeft(const FInputActionValue& Value)
{
	bool bPressed = Value.Get<bool>();
	if (bPressed)
	{
		RequestedDirection = ESnakeDirection::Left;
	}
}

void ASnakePawn::Input_TryTurnRight(const FInputActionValue& Value)
{
	bool bPressed = Value.Get<bool>();
	if (bPressed)
	{
		RequestedDirection = ESnakeDirection::Right;
	}
}

FVector ASnakePawn::GetVectorFromDirection(const ESnakeDirection Direction) const
{
	switch (Direction)
	{
		case ESnakeDirection::Up:		return FVector::ForwardVector;  // +X
		case ESnakeDirection::Down:		return -FVector::ForwardVector; // -X
		case ESnakeDirection::Left:		return -FVector::RightVector;   // -Y
		case ESnakeDirection::Right:	return FVector::RightVector;    // +Y
		default:						return FVector::ZeroVector;     // Should never happen, but we return zero just in case
	}
}

bool ASnakePawn::IsValidTurn(ESnakeDirection NewDirection) const
{
	// A turn is valid if it's not the same as the current direction, and it's not directly opposite to the current direction (e.g. if we're moving up, we can't turn down)
	if (NewDirection == CurrentDirection)
	{
		return false;
	}
	switch (CurrentDirection)
	{
	case ESnakeDirection::Up:
		return NewDirection != ESnakeDirection::Down;
	case ESnakeDirection::Down:
		return NewDirection != ESnakeDirection::Up;
	case ESnakeDirection::Left:
		return NewDirection != ESnakeDirection::Right;
	case ESnakeDirection::Right:
		return NewDirection != ESnakeDirection::Left;
	default:
		return false; // Should never happen, but we return false just in case
	}
}

void ASnakePawn::UpdateDirection(ESnakeDirection NewDirection)
{
	switch (NewDirection)
	{
		case ESnakeDirection::Up:	SetActorRotation(FRotator(0.f, 0.f, 0.f));
			break;
		case ESnakeDirection::Down:	SetActorRotation(FRotator(0.f, 180.f, 0.f));
			break;
		case ESnakeDirection::Left:	SetActorRotation(FRotator(0.f, -90.f, 0.f));
			break;
		case ESnakeDirection::Right:SetActorRotation(FRotator(0.f, 90.f, 0.f));
			break;
	}
}

void ASnakePawn::HandleDirectionChange()
{
	if (CurrentDirection != RequestedDirection && IsValidTurn(RequestedDirection))
	{
		CurrentDirection = RequestedDirection;
		UpdateDirection(CurrentDirection);
		// UE_LOG(LogTemp, Warning, TEXT("Direction changed to: %s"), *UEnum::GetValueAsString(CurrentDirection));
	}
}
FIntPoint ASnakePawn::DirectionToGridOffset(ESnakeDirection Direction) const
{
	switch (Direction)
	{
	case ESnakeDirection::Up: return FIntPoint(1, 0);	// +X
	case ESnakeDirection::Down: return FIntPoint(-1, 0);// -X
	case ESnakeDirection::Left: return FIntPoint(0, -1);// -Y
	case ESnakeDirection::Right: return FIntPoint(0, 1);// +Y
	default: return FIntPoint(0, 0);
	}
}

FVector ASnakePawn::GridToWorldLocation(const FIntPoint& GridPosition) const
{
	float TargetZ = 0.0f;
	if (GridGen)
	{
		int32 Idx = GridGen->GetIndex(GridPosition.X, GridPosition.Y, CurrentLayer);
		if (GridGen->GridData.IsValidIndex(Idx))
		{
			TargetZ = GridGen->GridData[Idx].ZOffset;
		}
	}
	return FVector(GridPosition.X * TileSize, GridPosition.Y * TileSize, TargetZ + VerticalOffset);
}

bool ASnakePawn::WouldHitWall(const FIntPoint& NextCell) const
{
	if (!GridGen) return true;

	//Check Map Bounds
	if (!GridGen->IsInBounds(NextCell.X, NextCell.Y)) return true;

	// Check for "Blocked" tiles in grid
	int32 Idx = GridGen->GetIndex(NextCell.X, NextCell.Y, CurrentLayer);
	if (GridGen->GridData.IsValidIndex(Idx))
	{
		return GridGen->GridData[Idx].CellType == ECellType::Blocked;
	}

	return false;
}

void ASnakePawn::HandleSnakeDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	// UE_LOG(LogTemp, Warning, TEXT("Snake has hit a wall and died!"));

	// Pauses for a moment, then reset snake to starting position and direction for now:
	// We can do a delay and call the ResetSnake method like this:
	GetWorldTimerManager().SetTimer(ResetTimerHandle, this, &ASnakePawn::ResetSnake, 1.f, false); // 1 second delay, not looping
}

void ASnakePawn::ResetSnake()
{
	const FIntPoint SpawnCell = GetClampedStartGridPosition();
	CurrentGridLocation = SpawnCell;
	PendingNextGridLocation = SpawnCell;
	SnakeHistory.Empty();

	CurrentDirection = ESnakeDirection::Right;
	RequestedDirection = ESnakeDirection::Right;
	UpdateDirection(CurrentDirection);
	
	const FVector ResetLocation = GridToWorldLocation(CurrentGridLocation);
	SetActorLocation(ResetLocation);
	StepStartWorldLocation = ResetLocation;
	StepTargetWorldLocation = ResetLocation;
	MoveInterpolationProgress = 0.f;
	bIsMovingToTarget = false;
	bIsDead = false;
}

FIntPoint ASnakePawn::GetClampedStartGridPosition() const
{
	return FIntPoint(
		FMath::Clamp(0, 1, GridGen->Width - 2),
		FMath::Clamp(0, 1, GridGen->Height - 2)
	);
}

TArray<FIntPoint> ASnakePawn::GetOccupiedCells()
{
	TArray<FIntPoint> OccupiedCells;
	for (const FVector& Loc : SegmentLocations)
	{
		OccupiedCells.Add(FIntPoint(
			FMath::RoundToInt(Loc.X / TileSize),
			FMath::RoundToInt(Loc.Y / TileSize)
		));
	}
	return OccupiedCells;
}

void ASnakePawn::DrawDebugInfo()
{
	if (bIsDrawDebugInfo && CollisionComponent)
	{
		DrawDebugSphere(
			GetWorld(),
			CollisionComponent->GetComponentLocation(),
			CollisionComponent->GetScaledSphereRadius(),
			16,
			FColor::Green,
			false,
			-1.f, // Duration <0 means for one frame
			0,
			2.0f // Line thickness
		);
	}

	// Draw pawn's forward vector arrow (Blue)
	FVector ForwardStart = GetActorLocation();
	FVector ForwardEnd = ForwardStart + GetActorForwardVector() * 100.f; // 100 units in front
	DrawDebugDirectionalArrow(GetWorld(), ForwardStart, ForwardEnd, 50.f, FColor::Blue, false, -1.f, 0, 3.0f);

	// Draw pawn's right vector arrow (Red)
	FVector RightStart = GetActorLocation();
	FVector RightEnd = RightStart + GetActorRightVector() * 100.f; // 100 units to the right
	DrawDebugDirectionalArrow(GetWorld(), RightStart, RightEnd, 50.f, FColor::Red, false, -1.f, 0, 3.0f);
}

void ASnakePawn::UpdateBodyVisuals()
{
	for (int32 i = 0; i < BodyParts.Num(); i++)
	{
		// calculate which index in history belongs to which segment, i.e. 0 is neck etc...
		int32 HistoryIndex = (i + 1) * PointsPerSegment;
		
		if (SnakeHistory.IsValidIndex(HistoryIndex))
		{
			FVector DesiredPosition = SnakeHistory[HistoryIndex];
			BodyParts[i]->SetWorldLocation(DesiredPosition);
			
			// point segment at the one before it
			FVector TargetForRotation;
			if (i == 0)
			{
				TargetForRotation = GetActorLocation();
			}
			else
			{
				int32 PreviousHistoryIndex = i * PointsPerSegment;
				TargetForRotation = SnakeHistory[PreviousHistoryIndex];
			}
			
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(DesiredPosition, TargetForRotation);
			
			// body segment static meshes need to be rotated 90 degrees
			FQuat OffsetQuat = FQuat(FRotator(0, 90, 0));
			BodyParts[i]->SetWorldRotation(LookAtRotation.Quaternion() * OffsetQuat);
			
			// animation fun squash and stretch
			float ActualDistance = FVector::Dist(DesiredPosition, TargetForRotation);
			BodyParts[i]->SetWorldScale3D(FVector(1.0f, ActualDistance / TileSize, 1.0f));
		}
	}
}
