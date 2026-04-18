// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "SnakePlayerState.h"
#include "SnakePlayerController.h"
#include "SnakeGameInstance.h"
#include "SnakeGameState.h"

ALobbyGameMode::ALobbyGameMode()
{
	GameStateClass = ASnakeGameState::StaticClass();
	PlayerStateClass = ASnakePlayerState::StaticClass();
	PlayerControllerClass = ASnakePlayerController::StaticClass();
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	USnakeGameInstance* GI = Cast<USnakeGameInstance>(GetGameInstance());
	if (ASnakeGameState* GS = GetGameState<ASnakeGameState>())
	{
		if (GI)
		{
			GS->ServerDisplayIP = GI->HostIPAddress;
		}
	}
}

FString ALobbyGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	// Parse the "Name" option
	FString InName = UGameplayStatics::ParseOption(Options, TEXT("PlayerName"));
	
	// Debug log to see what the Server is actually seeing in the URL
	// UE_LOG(LogTemp, Warning, TEXT("SERVER RECEIVED NAME: %s"), *InName);

	ASnakePlayerState* PS = NewPlayerController->GetPlayerState<ASnakePlayerState>();
	if (PS)
	{
		if (!InName.IsEmpty())
		{
			PS->Server_SetSnakeName(InName);
		}
		else
		{
			PS->SnakeName = TEXT("UnknownSnake");
		}
		PS->OnRep_SnakeName(); 
	}

	return ErrorMessage;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayerController)
{
	Super::PostLogin(NewPlayerController);

	if (NewPlayerController)
	{
		// 1. Handle GameState logic (Server-side)
		ASnakeGameState* SnakeGameState = GetGameState<ASnakeGameState>();
		if (SnakeGameState)
		{
			USnakeGameInstance* GI = Cast<USnakeGameInstance>(GetGameInstance());
			if (GI)
			{
				SnakeGameState->HostPlayerName = GI->UserPlayerName;
			}
		}

		// 2. Tell the specific player who just joined to show their UI
		// We use the RPC because UI must be spawned on the machine that owns the Controller
		ASnakePlayerController* PC = Cast<ASnakePlayerController>(NewPlayerController);
		if (PC && ULobbyUserWidget)
		{
			PC->Client_ShowLobbyUI(ULobbyUserWidget);
		}
	}
}

void ALobbyGameMode::StartGame(EPlayMode PlayMode)
{
	if (!HasAuthority()) return;
	
	bUseSeamlessTravel = true;

	FString MapName = (PlayMode == EPlayMode::BattleRoyale) ? TEXT("Lvl_SnakeBattle1") : TEXT("Lvl_Coop1");
    
	// ServerTravel handles moving ALL connected clients
	GetWorld()->ServerTravel(MapName + TEXT("?listen"), true);
}