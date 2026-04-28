// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SnakeGameState.h"
#include "BattleSnakeGameState.generated.h"

UCLASS()
class ABattleSnakeGameState : public ASnakeGameState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 AlivePlayers;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bMatchStarted = false;
	
	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 ItemSpawnCountLevel1 = 25;
    
	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 ItemSpawnCountLevel2 = 15;
    
	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 ItemSpawnCountLevel3 = 10;
	
	UPROPERTY(Replicated, BlueprintReadWrite)
	float PlayerMovementAdjustment1 = -0.1f;
    
	UPROPERTY(Replicated, BlueprintReadWrite)
	float PlayerMovementAdjustment2 = -0.2f;
    
	UPROPERTY(Replicated, BlueprintReadWrite)
	float PlayerMovementAdjustment3 = -0.3f;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;
};
