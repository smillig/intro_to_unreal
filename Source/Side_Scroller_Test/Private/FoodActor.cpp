// Fill out your copyright notice in the Description page of Project Settings.


#include "FoodActor.h"
#include "SnakePawn.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"

// Sets default values
AFoodActor::AFoodActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	CollisionSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere Component"));
	
	RootComponent = CollisionSphereComponent;
	
	CollisionSphereComponent->SetSphereRadius(25.0f);
	CollisionSphereComponent->SetCollisionProfileName(TEXT("Food"));
	
	FoodStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Food StaticMesh Component"));

	FoodStaticMeshComponent->SetupAttachment(RootComponent);
	FoodStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	CollisionSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AFoodActor::OnOverlap);
	
	SpawnNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpawnNiagara"));
	SpawnNiagara->SetupAttachment(RootComponent);
	SpawnNiagara->bAutoActivate = false;
	
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bAutoActivate = false;
}
void AFoodActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Mushrooms stays on board for only a set time
	if (HasAuthority() && FoodType == EFoodType::Mushroom)
	{
		SetLifeSpan(LifeTime);
	}
	// particle effects
	if (SpawnNiagaraEffect && SpawnNiagara)
	{
		SpawnNiagara->SetAsset(SpawnNiagaraEffect);
		SpawnNiagara->Activate(true);
	}
	// sound setup
	if (EatenSound && AudioComponent)
	{
		AudioComponent->SetSound(EatenSound);
	}
}

void AFoodActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASnakePawn* SnakePawn = Cast<ASnakePawn>(OtherActor))
	{
		if (EatenSound && AudioComponent)
		{
			AudioComponent->Play();
			AudioComponent->OnAudioFinished.AddDynamic(this, &AFoodActor::OnSoundFinished);
			// Disable collision and hide mesh
			CollisionSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			FoodStaticMeshComponent->SetVisibility(false);

			// Notify GameMode
			OnFoodEaten.Broadcast(this, SnakePawn);
		}
		else
		{
			// broadcast the food was eaten by whom
            OnFoodEaten.Broadcast(this, SnakePawn);
            // destroy food once eaten (Collided)
            Destroy();
		}
	}
}

void AFoodActor::OnSoundFinished()
{
	// Unbind to prevent multiple calls
	AudioComponent->OnAudioFinished.RemoveDynamic(this, &AFoodActor::OnSoundFinished);

	// Destroy after sound finishes
	Destroy();
}

