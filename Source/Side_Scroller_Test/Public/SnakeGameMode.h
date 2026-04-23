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
	
	UFUNCTION()
	void HandleFoodEaten(AFoodActor* EatenFood, ASnakePawn* SnakePawn);
	
	UFUNCTION()
	void OnMatchTimerTick();
	
	void EndGame();
	
	UPROPERTY(EditDefaultsOnly, Category = "Game Rules")
	int32 MaxFoodOnBoard = 5;
	
	UPROPERTY(EditDefaultsOnly, Category = "Game Rules")
	int32 MatchDurationSeconds = 180;
	
	FTimerHandle FoodSpawnTimerHandle;
	FTimerHandle MatchTimerHandle;
	
	UPROPERTY()
	AAGridGenerator* GridGen;

	int32 SpawnSlot = 0;
	
public:
	UFUNCTION(BlueprintCallable)
	void MaintainFoodCount();
	
	UFUNCTION(BlueprintCallable)
	void SpawnFood();
	
	FIntPoint GetNextSpawnCell();
	
	bool IsCellSafe(class ASnakePawn* MovingSnake, FIntPoint TargetCell);
	
};
