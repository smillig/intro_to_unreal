// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePawn.h"
#include "AGridGenerator.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
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
	// HeadMesh->SetHiddenInGame(true);

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

	SnakeSpline = CreateDefaultSubobject<USplineComponent>(TEXT("SnakeSpline"));
	SnakeSpline->SetupAttachment(RootComponent);
	// sets so location is updated through UpdateSplineVisuals
	SnakeSpline->SetUsingAbsoluteLocation(true);
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
	// HeadMesh->SetHiddenInGame(true);
	
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
	
	CurrentDirection = ESnakeDirection::Up;
	RequestedDirection = ESnakeDirection::Up;
	SetActorRotation(FRotator(0.f, -90.f, 0.f));
	HeadStartRotation = GetActorRotation();
	HeadTargetRotation = GetActorRotation();
	HandleDirectionChange();
	
	// player starts moving immediatly
	bIsMovingToTarget = false;
	MoveTimer = MoveInterval;
}

// Called every frame
void ASnakePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead || !GridGen || !SnakeSpline) return;

	// Run the movement logic every frame for smooth interpolation
	GridMove(DeltaTime);
	
	// breadcrumbs for snake history
	FVector CurrentHeadLocation = GetActorLocation();

	if (SnakeHistory.Num() < 2)
	{
		SnakeHistory.Empty();
		SnakeHistory.Add(CurrentHeadLocation); // Index 0: The "Live" Head
		SnakeHistory.Add(CurrentHeadLocation); // Index 1: The first breadcrumb
	}
	
	SnakeHistory[0] = CurrentHeadLocation;
	
	// Breadcrumbs: Check distance between current head (0) and last recorded breadcrumb (1)
	if (FVector::Dist(SnakeHistory[0], SnakeHistory[1]) >= 10.0f)
	{
		// Insert a new breadcrumb at index 1
		SnakeHistory.Insert(CurrentHeadLocation, 1);
	}

	// sync spline to history
	SnakeSpline->ClearSplinePoints(false);
	for (int32 i = 0; i < SnakeHistory.Num(); i++)
	{
		SnakeSpline->AddSplinePoint(SnakeHistory[i], ESplineCoordinateSpace::World, false);
		// Changed to Curve for smoother visuals
		SnakeSpline->SetSplinePointType(i, ESplinePointType::Curve, false);
	}
	SnakeSpline->UpdateSpline();

	// Trim history based on current segment count
	float RequiredLength = (SegmentCount + 1) * TileSize;
    
	while (SnakeSpline->GetSplineLength() > RequiredLength && SnakeHistory.Num() > 2)
	{
		SnakeHistory.Pop();
		// We must update the spline inside the loop to get the new length
		SnakeSpline->ClearSplinePoints(false);
		for (int32 i = 0; i < SnakeHistory.Num(); i++)
		{
			SnakeSpline->AddSplinePoint(SnakeHistory[i], ESplineCoordinateSpace::World, false);
		}
		SnakeSpline->UpdateSpline();
	}
    
	UpdateSplineVisuals();
	
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
		HeadStartRotation = HeadMesh->GetComponentRotation();
		bIsMovingToTarget = true;
	}

	MoveInterpolationProgress += DeltaTime / MoveInterval;
	const float Alpha = FMath::Clamp(MoveInterpolationProgress, 0.f, 1.f);

	// smoothly rotate head to target using quaternions for shortest path
	FQuat QStart = HeadStartRotation.Quaternion();
	FQuat QTarget = HeadTargetRotation.Quaternion();
	FQuat QInterp = FQuat::Slerp(QStart, QTarget, Alpha);
	HeadMesh->SetWorldRotation(QInterp);
	// Use Lerp for constant speed across the cell
	const FVector NewLocation = FMath::Lerp(StepStartWorldLocation, StepTargetWorldLocation, Alpha);
	SetActorLocation(NewLocation, false);

	if (Alpha >= 1.f)
	{
		HeadMesh->SetWorldRotation(HeadTargetRotation);
		// Update Segment logic positions
		for (int32 i = SegmentLocations.Num() - 1; i > 0; i--) {
			SegmentLocations[i] = SegmentLocations[i - 1];
		}
		SegmentLocations[0] = StepStartWorldLocation; // Where the head just came from

		
		// reached the target grid location
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
    
	// Logic: Add a location at the very end
	FVector LastLoc = SegmentLocations.Num() > 0 ? SegmentLocations.Last() : GetActorLocation();
	SegmentLocations.Add(LastLoc);
    
	SegmentCount++;
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
		case ESnakeDirection::Up:	HeadTargetRotation = FRotator(0.f, -90.f, 0.f);
			break;
		case ESnakeDirection::Down:	HeadTargetRotation = FRotator(0.f, 90.f, 0.f);
			break;
		case ESnakeDirection::Left:	HeadTargetRotation = FRotator(0.f, 180.f, 0.f);
			break;
		case ESnakeDirection::Right:HeadTargetRotation = FRotator(0.f, 0.f, 0.f);
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

	CurrentDirection = ESnakeDirection::Right;
	RequestedDirection = ESnakeDirection::Right;
	UpdateDirection(CurrentDirection);
	
	const FVector ResetLocation = GridToWorldLocation(CurrentGridLocation);
	SetActorLocation(ResetLocation);
	StepStartWorldLocation = ResetLocation;
	StepTargetWorldLocation = ResetLocation;
	MoveInterpolationProgress = 0.f;
	BodyParts.Empty();
	TArray<USceneComponent*> SnakeSplineChildren;
	SnakeSpline->GetChildrenComponents(true, SnakeSplineChildren);
	for (USplineMeshComponent* SnakeSplineMeshComponent : SplineMeshParts)
	{
		if (SnakeSplineMeshComponent) SnakeSplineMeshComponent->DestroyComponent();
	}
	SnakeHistory.Empty();
    SegmentLocations.Empty();
    SegmentLocations.Add(ResetLocation);
	SegmentLocations.Add(ResetLocation); // buffer extra slot just in case
	SegmentCount = 0;
	SplineMeshParts.Empty();
	SnakeSpline->ClearSplinePoints(true);
	SnakeSpline->UpdateSpline();
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

void ASnakePawn::UpdateSplineVisuals()
{
	int32 TotalVisualParts = SegmentCount;
    float CurrentSplineLength = SnakeSpline->GetSplineLength();

    while (SplineMeshParts.Num() < TotalVisualParts)
    {
        USplineMeshComponent* NewMesh = NewObject<USplineMeshComponent>(this);
        NewMesh->SetForwardAxis(ESplineMeshAxis::X, false);
        NewMesh->SetMobility(EComponentMobility::Movable);
        NewMesh->AttachToComponent(SnakeSpline, FAttachmentTransformRules::KeepRelativeTransform);
        NewMesh->RegisterComponent();
        SplineMeshParts.Add(NewMesh);
    }

    for (int32 i = 0; i < SplineMeshParts.Num(); i++)
    {
        USplineMeshComponent* SMC = SplineMeshParts[i];

        // Hide parts that are beyond our current segment count OR beyond current spline length
        float StartDist = i * TileSize;
        float EndDist = (i + 1) * TileSize;

        if (i >= TotalVisualParts || StartDist > CurrentSplineLength)
        {
            SMC->SetVisibility(false);
            continue;
        }

        SMC->SetVisibility(true);

        // Assign Mesh
        UStaticMesh* SelectedMesh = BodyMeshAsset;
        // if (i == 0) SelectedMesh = HeadMeshAsset;
        if (i == TotalVisualParts - 1 && SegmentCount > 0) SelectedMesh = TailMeshAsset;
        
        if (SMC->GetStaticMesh() != SelectedMesh) SMC->SetStaticMesh(SelectedMesh);

        // Spline calculations
        FVector StartPos = SnakeSpline->GetLocationAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
        FVector StartTangent = SnakeSpline->GetTangentAtDistanceAlongSpline(StartDist, ESplineCoordinateSpace::World);
        FVector EndPos = SnakeSpline->GetLocationAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::World);
        FVector EndTangent = SnakeSpline->GetTangentAtDistanceAlongSpline(EndDist, ESplineCoordinateSpace::World);

        FTransform CombinedTransform = SMC->GetComponentTransform();
        FVector LocalStart = CombinedTransform.InverseTransformPosition(StartPos);
        FVector LocalStartTangent = CombinedTransform.InverseTransformVector(StartTangent);
        FVector LocalEnd = CombinedTransform.InverseTransformPosition(EndPos);
        FVector LocalEndTangent = CombinedTransform.InverseTransformVector(EndTangent);

        // Clamp tangents to TileSize to prevent the mesh from "exploding" on sharp turns
        SMC->SetStartAndEnd(LocalStart, LocalStartTangent.GetClampedToMaxSize(TileSize), 
                            LocalEnd, LocalEndTangent.GetClampedToMaxSize(TileSize), true);
	}
}
