// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopSnakeGameState.h"
#include "Net/UnrealNetwork.h"

void ACoopSnakeGameState::BeginPlay()
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

void ACoopSnakeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACoopSnakeGameState, ItemSpawnCountLevel1);
	DOREPLIFETIME(ACoopSnakeGameState, ItemSpawnCountLevel2);
	DOREPLIFETIME(ACoopSnakeGameState, ItemSpawnCountLevel3);
	DOREPLIFETIME(ACoopSnakeGameState, PlayerMovementAdjustment1);
	DOREPLIFETIME(ACoopSnakeGameState, PlayerMovementAdjustment2);
	DOREPLIFETIME(ACoopSnakeGameState, PlayerMovementAdjustment3);
	
}