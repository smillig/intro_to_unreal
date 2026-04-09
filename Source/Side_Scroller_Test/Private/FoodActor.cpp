// Fill out your copyright notice in the Description page of Project Settings.


#include "FoodActor.h"

#include "SnakeGameMode.h"
#include "SnakePawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

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
}

void AFoodActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASnakePawn* SnakePawn = Cast<ASnakePawn>(OtherActor))
	{
		// SnakePawn->AddSegment(); // could pass snake ScoreValue to grow or shrink
		UE_LOG(LogTemp, Log, TEXT("Ate a food worth %d points"), ScoreValue);
		// spawn new food
		if (ASnakeGameMode* GM = Cast<ASnakeGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->SpawnFood();
		}
		// destroy food once eaten (Collided)
		Destroy();
	}
}

