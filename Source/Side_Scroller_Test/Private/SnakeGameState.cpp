// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameState.h"
#include "Net/UnrealNetwork.h"

void ASnakeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASnakeGameState, ServerDisplayIP);
}
