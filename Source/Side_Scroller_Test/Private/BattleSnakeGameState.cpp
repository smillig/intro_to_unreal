// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSnakeGameState.h"
#include "Net/UnrealNetwork.h"

void ABattleSnakeGameState::BeginPlay()
{
	Super::BeginPlay();
	
	switch (CurrentLevel)
	{
	case 1:
		{
			CurrentMovementAdjustment = PlayerMovementAdjustment1;
			break;
		}
	case 2:
		{
			CurrentMovementAdjustment = PlayerMovementAdjustment2;
			break;
		}
	case 3:
		{
			CurrentMovementAdjustment = PlayerMovementAdjustment3;
			break;
		}
	default:
		{
			CurrentMovementAdjustment = PlayerMovementAdjustment1;
			break;
		}
	}
}

void ABattleSnakeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABattleSnakeGameState, AlivePlayers);
	DOREPLIFETIME(ABattleSnakeGameState, bMatchStarted);
	DOREPLIFETIME(ABattleSnakeGameState, ItemSpawnCountLevel1);
	DOREPLIFETIME(ABattleSnakeGameState, ItemSpawnCountLevel2);
	DOREPLIFETIME(ABattleSnakeGameState, ItemSpawnCountLevel3);
	DOREPLIFETIME(ABattleSnakeGameState, PlayerMovementAdjustment1);
	DOREPLIFETIME(ABattleSnakeGameState, PlayerMovementAdjustment2);
	DOREPLIFETIME(ABattleSnakeGameState, PlayerMovementAdjustment3);
	
}