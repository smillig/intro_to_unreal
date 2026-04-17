// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "SnakeGameInstance.h"

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (ULobbyUserWidget)
	{
		UUserWidget* Menu = CreateWidget<UUserWidget>(GetWorld(), ULobbyUserWidget);
		if (Menu) Menu->AddToViewport();
		// Optional: show mouse cursor
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeUIOnly());
		}
	}
}


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