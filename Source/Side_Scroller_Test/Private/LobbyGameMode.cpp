// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "SnakePlayerState.h"
#include "SnakePlayerController.h"
#include "SnakeGameInstance.h"
#include "SnakeGameState.h"

ALobbyGameMode::ALobbyGameMode()
{
	GameStateClass = ASnakeGameState::StaticClass();
	PlayerStateClass = ASnakePlayerState::StaticClass();
	PlayerControllerClass = ASnakePlayerController::StaticClass();
	
	bUseSeamlessTravel = false;
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	USnakeGameInstance* GI = Cast<USnakeGameInstance>(GetGameInstance());
	APlayerController* HostPC = GetWorld()->GetFirstPlayerController();
	ASnakeGameState* GS = GetGameState<ASnakeGameState>();
	ASnakePlayerState* PS = HostPC->GetPlayerState<ASnakePlayerState>();
	if (GI)
	{
		if (HostPC && PS)
		{
			PS->SnakeName = GI->UserPlayerName;
			PS->OnRep_SnakeName();
		}
		if (GS)
		{
			GS->ServerDisplayIP = GI->HostIPAddress;
		}
	}
	// bUseSeamlessTravel = true;
}

FString ALobbyGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	
	// Debug log to see what the Server is actually seeing in the URL
	// UE_LOG(LogTemp, Warning, TEXT("SERVER RECEIVED NAME: %s"), *InName);

	// Parse the name from the URL
	FString InName = UGameplayStatics::ParseOption(Options, TEXT("PlayerName"));
	if (InName.IsEmpty()) InName = TEXT("GuestSnake");

	// Store the name in our map using the UniqueID as the key
	if (NewPlayerController)
	{
		PendingNames.Add(NewPlayerController, InName);
	}

	return ErrorMessage;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayerController)
{
	Super::PostLogin(NewPlayerController);

	if (!NewPlayerController) return;

	// Retrieve the name we stored in InitNewPlayer
	if (FString* FoundName = PendingNames.Find(NewPlayerController))
	{
		ASnakePlayerState* PS = NewPlayerController->GetPlayerState<ASnakePlayerState>();
		if (PS)
		{
			PS->SnakeName = *FoundName;
			PS->OnRep_SnakeName();
		}
		// Clean up
		PendingNames.Remove(NewPlayerController);
	}
	
	// Tell the specific player who just joined to show their UI
	// Use the RPC because UI must be spawned on the machine that owns the Controller
	ASnakePlayerController* PC = Cast<ASnakePlayerController>(NewPlayerController);
	if (PC && ULobbyUserWidget)
	{
		PC->Client_ShowLobbyUI(ULobbyUserWidget);
	}
}

void ALobbyGameMode::StartGame(EPlayMode PlayMode)
{
	if (!HasAuthority()) return;

	FString MapName = (PlayMode == EPlayMode::BattleRoyale) ? TEXT("Lvl_SnakeBattle1") : TEXT("Lvl_Coop1");
    
	// ServerTravel handles moving ALL connected clients
	GetWorld()->ServerTravel(MapName + TEXT("?listen"), true);
}