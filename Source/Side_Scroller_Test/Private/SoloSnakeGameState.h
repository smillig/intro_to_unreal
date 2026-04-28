// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SnakeGameState.h"
#include "SoloSnakeGameState.generated.h"


UCLASS()
class ASoloSnakeGameState : public ASnakeGameState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 ItemSpawnCountLevel1 = 35;
    
	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 ItemSpawnCountLevel2 = 20;
    
	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 ItemSpawnCountLevel3 = 15;
	
	UPROPERTY(Replicated, BlueprintReadWrite)
	float PlayerMovementAdjustment1 = -0.075f;
    
	UPROPERTY(Replicated, BlueprintReadWrite)
	float PlayerMovementAdjustment2 = -0.1f;
    
	UPROPERTY(Replicated, BlueprintReadWrite)
	float PlayerMovementAdjustment3 = -0.15f;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;
};
