// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSnakeGameState.h"
#include "Net/UnrealNetwork.h"

void ABattleSnakeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABattleSnakeGameState, AlivePlayers);
	DOREPLIFETIME(ABattleSnakeGameState, bMatchStarted);
}