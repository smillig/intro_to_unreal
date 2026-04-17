// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SnakeGameState.generated.h"

/**
 * 
 */
UCLASS()
class ASnakeGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FString HostPlayerName = "Host";
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	FString ClientPlayerName = "";
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SetClientName(const FString& NewClientName);	
};
