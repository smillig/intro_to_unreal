// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SnakeGameInstance.h"
#include "GameFramework/GameState.h"
#include "SnakeGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerUpdated, int32, TimeRemaining);

UCLASS()
class ASnakeGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnTimerUpdated OnTimerUpdated;
	
	UPROPERTY(replicatedUsing = OnRep_MatchTimeRemaining, BlueprintReadOnly)
	int32 MatchTimeRemaining = 0;
	
	UFUNCTION()
	void OnRep_MatchTimeRemaining();
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	FString ServerDisplayIP = "127.0.0.1:7777";
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	float CurrentMovementAdjustment = 0.0f;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 CurrentLevel = 1;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	EPlayMode CurrentPlayMode = EPlayMode::None;
};
