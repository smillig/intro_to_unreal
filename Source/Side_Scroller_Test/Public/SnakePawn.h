// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "SnakePawn.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class USplineComponent;
class USplineMeshComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AAGridGenerator;
struct FGridCell;

UENUM(BlueprintType)
enum class ESnakeDirection : uint8
{
	Up,
	Down,
	Left,
	Right
};

UCLASS()
class SIDE_SCROLLER_TEST_API ASnakePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASnakePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionComponent;
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
	
	UPROPERTY(VisibleAnywhere, Category = "Snake Visuals")
	UStaticMeshComponent* HeadMeshComponent;
	
	UPROPERTY(EditAnywhere, Category = "Snake Visuals")
	UStaticMesh* HeadMeshAsset;
	
	UPROPERTY(EditAnywhere, Category = "Snake Visuals")
	UStaticMesh* BodyMeshAsset;

	UPROPERTY(EditAnywhere, Category = "Snake Visuals")
	UStaticMesh* TailMeshAsset;
	
	UPROPERTY(VisibleAnywhere, Category = "Snake Visuals")
	USplineComponent* SnakeSpline;

	UPROPERTY()
	TArray<USplineMeshComponent*> SplineMeshParts;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* SnakeMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TurnUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TurnDownAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TurnLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TurnRightAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> PauseAction;
	
	// Seconds per grid step
	UPROPERTY(EditAnywhere, Category = "Snake Logic")
	float MoveInterval = 0.5f;
	
	// Needs to be same size as grid TileSize
	UPROPERTY(EditAnywhere, Category = "Snake Logic")
	float TileSize = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bIsDrawDebugInfo = false;
	
	UPROPERTY()
	AAGridGenerator* GridGen;
	// Grid coordinates of snake
	// might need to move or reference in grid for server auth
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Snake Logic")
	TArray<FVector> SegmentLocations; 
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Snake Logic")
	int32 SegmentCount = 0;
	
	TArray<FVector> SnakeHistory;
	TArray<UStaticMeshComponent*> BodyParts;
	float VerticalOffset = 65.0f;

	float MoveTimer;
	int32 CurrentLayer = 0;
	bool bIsMovementLocked = false;
	UPROPERTY(ReplicatedUsing= OnRep_CurrentDirection, BlueprintReadOnly, Category = "Snake Logic")
	ESnakeDirection CurrentDirection = ESnakeDirection::Up;
	UFUNCTION()
	void OnRep_CurrentDirection();
	ESnakeDirection RequestedDirection = ESnakeDirection::Up;
	FIntPoint CurrentGridLocation = FIntPoint(0, 0);
	FIntPoint PendingNextGridLocation = FIntPoint(0, 0);
	FVector StepStartWorldLocation = FVector::ZeroVector;
	FVector StepTargetWorldLocation = FVector::ZeroVector;
	FRotator HeadStartRotation;
	FRotator HeadTargetRotation;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	float MoveInterpolationProgress = 0.f;
	bool bIsMovingToTarget = false;
	bool bIsDead = false;
	FTimerHandle ResetTimerHandle;

	FVector GetVectorFromDirection(ESnakeDirection Direction) const;
	FIntPoint DirectionToGridOffset(ESnakeDirection Direction) const;
	FVector GridToWorldLocation(const FIntPoint& GridPosition) const;
	
	void Input_TryTurnUp(const FInputActionValue& Value);
	void Input_TryTurnDown(const FInputActionValue& Value);
	void Input_TryTurnLeft(const FInputActionValue& Value);
	void Input_TryTurnRight(const FInputActionValue& Value);
	void Input_TryPause(const FInputActionValue& Value);
	
	void HandleDirectionChange();
	void UpdateDirection(ESnakeDirection NewDirection);
	bool IsValidTurn(ESnakeDirection NewDirection) const;
	void DrawDebugInfo();

	bool WouldHitWall(const FIntPoint& NextCell) const;
	void ResetSnake();
	
	FIntPoint GetClampedStartGridPosition() const;
	void GridMove(const float DeltaTime);
	void UpdateSplineVisuals();
	void CheckLayerTransition(int32 TargetX, int32 TargetY);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	TArray<FIntPoint> GetOccupiedCells();
	
	UPROPERTY(Replicated, BlueprintReadWrite,  Category = "Snake Logic")
	float MoveIntervalAdjustment = 0.0f;
	
	// For hooking in blueprints when eating food
	UFUNCTION(BlueprintCallable)
	void ModifySegments(int32 Amount);
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void PawnClientRestart() override;
	
	bool IsDead() const { return bIsDead; }
	void HandleSnakeDeath();

	UFUNCTION(Server, Reliable)
	void Server_SetRequestedDirection(ESnakeDirection NewDirection);
	
	UFUNCTION(Server, Reliable)
	void Server_RequestPause();
	
	UFUNCTION(Client, Reliable)
	void Client_ResetLogic(FVector NewLocation);

	UFUNCTION(Client, Reliable)
	void Client_SetAdjustSnakeSpeed(float SpeedOffset);
};
