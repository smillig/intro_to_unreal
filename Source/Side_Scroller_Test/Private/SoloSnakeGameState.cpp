// Fill out your copyright notice in the Description page of Project Settings.


#include "SoloSnakeGameState.h"
#include "Net/UnrealNetwork.h"


void ASoloSnakeGameState::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("SoloSnakeGameState::BeginPlay"));
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

void ASoloSnakeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASoloSnakeGameState, ItemSpawnCountLevel1);
	DOREPLIFETIME(ASoloSnakeGameState, ItemSpawnCountLevel2);
	DOREPLIFETIME(ASoloSnakeGameState, ItemSpawnCountLevel3);
	DOREPLIFETIME(ASoloSnakeGameState, PlayerMovementAdjustment1);
	DOREPLIFETIME(ASoloSnakeGameState, PlayerMovementAdjustment2);
	DOREPLIFETIME(ASoloSnakeGameState, PlayerMovementAdjustment3);
	
}
