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

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;
	
	// Seconds per grid step
	UPROPERTY(EditAnywhere, Category = "Snake Logic")
	float MoveInterval = 0.3f; 
	
	// Needs to be same size as grid TileSize
	UPROPERTY(EditAnywhere, Category = "Snake Logic")
	float TileSize = 100.0f;

	// Snake game state
	FVector CurrentGridDir;
	FVector NextGridDir;
	// Grid coordinates of snake
	TArray<FVector> SegmentLocations; 
	TArray<UStaticMeshComponent*> BodyParts;

	float MoveTimer;
	int32 CurrentLayer = 0;
	bool bIsMovementLocked = false;

	void HandleMove(const FInputActionValue& Value);
	void PerformGridStep();
	void UpdateVisuals(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// For hooking in blueprints when eating food
	UFUNCTION(BlueprintCallable)
	void AddSegment();

};
