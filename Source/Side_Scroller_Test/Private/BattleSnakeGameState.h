// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SnakeGameState.h"
#include "BattleSnakeGameState.generated.h"

/**
 * 
 */
UCLASS()
class ABattleSnakeGameState : public ASnakeGameState
{
	GENERATED_BODY()
public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 AlivePlayers;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bMatchStarted = false;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
