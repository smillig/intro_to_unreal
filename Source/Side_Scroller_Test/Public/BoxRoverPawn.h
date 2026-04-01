// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "BoxRoverPawn.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class SIDE_SCROLLER_TEST_API ABoxRoverPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABoxRoverPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);
	void Turn(const FInputActionValue& Value);

	// Needed: Collision, Mesh, SpringArm, Camera, InputMappingContext, Input actions, moveSpeed, turnSpeed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Turn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TurnSpeed = 120.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	float MoveInput = 0.0f;
	float TurnInput = 0.0f;

};
