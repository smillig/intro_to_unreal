// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SnakePlayerState.generated.h"


UCLASS()
class SIDE_SCROLLER_TEST_API ASnakePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	// The variable we want to share across the network
	UPROPERTY(ReplicatedUsing = OnRep_SnakeName, BlueprintReadOnly, Category = "Snake")
	FString SnakeName;
	
	UPROPERTY(Replicated)
	int32 SpawnIndex = -1;

	UFUNCTION()
	void OnRep_SnakeName();

	// The RPC to set the name on the server
	UFUNCTION(Server, Reliable)
	void Server_SetSnakeName(const FString& NewName);

	// MANDATORY for replication to work
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
