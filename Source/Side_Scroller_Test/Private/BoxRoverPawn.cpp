// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxRoverPawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
ABoxRoverPawn::ABoxRoverPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	SetRootComponent(CollisionSphere);
	CollisionSphere->InitSphereRadius(50.0f);
	
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(CollisionSphere);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(CollisionSphere);
	SpringArm->TargetArmLength = 700.0f;
	SpringArm->SetRelativeRotation(FRotator(-25.0f, 0.0f, 0.0f));
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ABoxRoverPawn::BeginPlay()
{
	// Runtime initialization, adding input mapping contexts, references that depend on having a world/game state.
	Super::BeginPlay();

	// Set up player controls and mapping.
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

// Called every frame
void ABoxRoverPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FMath::IsNearlyZero(TurnInput))
	{
		AddActorLocalRotation(FRotator(0.0f, TurnInput * TurnSpeed * DeltaTime, 0.0f));
	}
	if (!FMath::IsNearlyZero(MoveInput))
	{
		const FVector DeltaForward = GetActorForwardVector() * MoveInput * MoveSpeed * DeltaTime;
		AddActorLocalOffset(DeltaForward, true);
	}
	// Update, check movement, turning...
}

// Called to bind functionality to input
void ABoxRoverPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind actions. Using Cast is safer than CastChecked.
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IA_Move)
		{
			EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ABoxRoverPawn::Move);
			EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Completed, this, &ABoxRoverPawn::Move);
		}
		if (IA_Turn)
		{
			EnhancedInputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &ABoxRoverPawn::Turn);
			EnhancedInputComponent->BindAction(IA_Turn, ETriggerEvent::Completed, this, &ABoxRoverPawn::Turn);
		}
	}
}

void ABoxRoverPawn::Move(const FInputActionValue& Value)
{
	MoveInput = Value.Get<float>();
}

void ABoxRoverPawn::Turn(const FInputActionValue& Value)
{
	TurnInput = Value.Get<float>();
}
