// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleSnakeGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "SnakeGameInstance.h"
#include "BattleSnakeGameState.h"
#include "SnakePlayerState.h"
#include "SnakePlayerController.h"

ABattleSnakeGameMode::ABattleSnakeGameMode()
{
	// Point this GameMode to use your specific Battle classes
	GameStateClass = ABattleSnakeGameState::StaticClass();
	PlayerStateClass = ASnakePlayerState::StaticClass();
	PlayerControllerClass = ASnakePlayerController::StaticClass();
}

FString ABattleSnakeGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	FString InName = UGameplayStatics::ParseOption(Options, TEXT("PlayerName"));

	if (NewPlayerController)
	{
		if (InName.IsEmpty() && NewPlayerController->IsLocalController())
		{
			if (USnakeGameInstance* GI = GetGameInstance<USnakeGameInstance>())
			{
				InName = GI->UserPlayerName;
			}
		}

		// Fallback
		if (InName.IsEmpty()) InName = TEXT("UnknownSnakePlayer");

		PendingNames.Add(NewPlayerController, InName);
	}
	return ErrorMessage;
}

void ABattleSnakeGameMode::PostLogin(APlayerController* NewPlayerController)
{
	Super::PostLogin(NewPlayerController);
    
	// if relogging in or hard transition make sure name gets populated
	if (FString* FoundName = PendingNames.Find(NewPlayerController))
	{
		ASnakePlayerState* PS = NewPlayerController->GetPlayerState<ASnakePlayerState>();
		if (PS)
		{
			PS->SnakeName = *FoundName;
			PS->OnRep_SnakeName();
		}
		PendingNames.Remove(NewPlayerController);
	}
	
	if (NewPlayerController)
	{
		// Get the PlayerState that just traveled from the Lobby
		ASnakePlayerState* PS = NewPlayerController->GetPlayerState<ASnakePlayerState>();
		
		if (PS)
		{
			// Now log the actual name the player chose in the menu
			UE_LOG(LogTemp, Log, TEXT("Snake [%s] has successfully arrived in the Battle!"), *PS->SnakeName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("A player arrived, but their PlayerState is missing!"));
		}
	}
	
}