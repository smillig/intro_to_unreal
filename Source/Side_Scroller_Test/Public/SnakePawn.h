// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "SnakePawn.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USpringArmComponent;
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
	UStaticMeshComponent* HeadMesh;
	
	UPROPERTY(EditAnywhere, Category = "Snake Visuals")
	UStaticMesh* BodyMeshAsset;

	UPROPERTY(EditAnywhere, Category = "Snake Visuals")
	UStaticMesh* TailMeshAsset;
	
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
	TArray<FVector> SegmentLocations; 
	TArray<UStaticMeshComponent*> BodyParts;
	float VerticalOffset = 65.0f;

	float MoveTimer;
	int32 CurrentLayer = 0;
	bool bIsMovementLocked = false;
	ESnakeDirection CurrentDirection = ESnakeDirection::Up;
	ESnakeDirection RequestedDirection = ESnakeDirection::Up;
	FIntPoint CurrentGridLocation = FIntPoint(0, 0);
	FIntPoint PendingNextGridLocation = FIntPoint(0, 0);
	FVector StepStartWorldLocation = FVector::ZeroVector;
	FVector StepTargetWorldLocation = FVector::ZeroVector;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
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
	
	void HandleDirectionChange();
	void UpdateDirection(ESnakeDirection NewDirection);
	bool IsValidTurn(ESnakeDirection NewDirection) const;
	void DrawDebugInfo();

	bool WouldHitWall(const FIntPoint& NextCell) const;
	void ResetSnake();
	void HandleSnakeDeath();
	FIntPoint GetClampedStartGridPosition() const;
	void GridMove(const float DeltaTime);
	void CheckLayerTransition(int32 TargetX, int32 TargetY);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	TArray<FIntPoint> GetOccupiedCells();
	
	// For hooking in blueprints when eating food
	UFUNCTION(BlueprintCallable)
	void AddSegment();

};
