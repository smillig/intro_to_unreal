// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FoodActor.generated.h"

// Declare the Delegate Signature
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFoodEatenSignature, class AFoodActor*, EatenFood, class ASnakePawn*, Eater);

class USphereComponent;
class UStaticMeshComponent;
class UAudioComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;

UENUM(BlueprintType)
enum class EFoodType : uint8 { Apple, Egg, Mushroom };

UCLASS()
class SIDE_SCROLLER_TEST_API AFoodActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFoodActor();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Food Properties")
	EFoodType FoodType = EFoodType::Apple;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Food Properties")
	int32 ScoreValue = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Food Properties")
	int32 GrowthAmount = 1;
	
	UPROPERTY(BlueprintAssignable)
	FOnFoodEatenSignature OnFoodEaten;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionSphereComponent;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FoodStaticMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* SpawnNiagaraEffect;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
	UNiagaraComponent* SpawnNiagara;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Food Properties")
	UAudioComponent* AudioComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* EatenSound;
	
	UFUNCTION()
	void OnSoundFinished();
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
					UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
					bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Food Properties")
	float LifeTime = 15.0f;
};
