// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SnakeGameMode.generated.h"

class AFoodActor;
class AAGridGenerator;
class ASnakePawn;

UCLASS()
class SIDE_SCROLLER_TEST_API ASnakeGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Game Rules")
	TSubclassOf<AFoodActor> AppleClass;
	
	UPROPERTY(EditAnywhere, Category = "Game Rules")
	TSubclassOf<AFoodActor> EggClass;
	
	UPROPERTY(EditAnywhere, Category = "Game Rules")
	TSubclassOf<AFoodActor> MushroomClass;
	
	UPROPERTY()
	AAGridGenerator* GridGen;
	
public:
	UFUNCTION(BlueprintCallable)
	void SpawnFood();
	
private:
	UPROPERTY()
	ASnakePawn* SnakePawn;
};
