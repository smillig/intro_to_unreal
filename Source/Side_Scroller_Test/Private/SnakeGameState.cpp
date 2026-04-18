// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameState.h"
#include "Net/UnrealNetwork.h"

void ASnakeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASnakeGameState, HostPlayerName);
	DOREPLIFETIME(ASnakeGameState, ClientPlayerName);
	DOREPLIFETIME(ASnakeGameState, ServerDisplayIP);
}

void ASnakeGameState::Server_SetClientName_Implementation(const FString& NewClientName)
{
	ClientPlayerName = NewClientName;
}
