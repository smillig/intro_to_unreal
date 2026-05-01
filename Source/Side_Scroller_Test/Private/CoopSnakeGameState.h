// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SnakeGameState.h"
#include "CoopSnakeGameState.generated.h"

/**
 * 
 */
UCLASS()
class ACoopSnakeGameState : public ASnakeGameState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 ItemSpawnCountLevel1 = 15;
    
	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 ItemSpawnCountLevel2 = 10;
    
	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 ItemSpawnCountLevel3 = 5;
	
	UPROPERTY(Replicated, BlueprintReadWrite)
	float PlayerMovementAdjustment1 = -0.1f;
    
	UPROPERTY(Replicated, BlueprintReadWrite)
	float PlayerMovementAdjustment2 = -0.25f;
    
	UPROPERTY(Replicated, BlueprintReadWrite)
	float PlayerMovementAdjustment3 = -0.4f;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;
};
