// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "SnakeGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	// You could update a PlayerList on the GameState here
}

void ALobbyGameMode::StartGame(EPlayMode PlayMode)
{
	if (USnakeGameInstance* GI = Cast<USnakeGameInstance>(GetGameInstance()))
	{
		GI->SelectedMode = PlayMode;
        
		// ServerTravel is critical: it carries all connected clients to the new level
		FString MapName = (PlayMode == EPlayMode::BattleRoyale) ? "Lvl_SnakeBattle1" : "Lvl_Coop1";
		GetWorld()->ServerTravel(MapName + "?Listen");
	}
}