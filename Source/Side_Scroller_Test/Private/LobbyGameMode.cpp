// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "SnakePlayerState.h"
#include "SnakeGameInstance.h"
#include "SnakeGameState.h"

ALobbyGameMode::ALobbyGameMode()
{
	GameStateClass = ASnakeGameState::StaticClass();
	PlayerStateClass = ASnakePlayerState::StaticClass(); 
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	ASnakeGameState* GameState = Cast<ASnakeGameState>(GetWorld()->GetGameState());
	if (GameState)
	{
		USnakeGameInstance* GI = Cast<USnakeGameInstance>(GetGameInstance());
		if (GI)
		{
			GameState->HostPlayerName = GI->HostPlayerName;
		}
	}
	
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

FString ALobbyGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	// Parse the "Name" option
	FString InName = UGameplayStatics::ParseOption(Options, TEXT("PlayerName"));
	
	// Debug log to see what the Server is actually seeing in the URL
	UE_LOG(LogTemp, Warning, TEXT("SERVER RECEIVED NAME: %s"), *InName);

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
	// No longer need to parse names here, InitNewPlayer handled it!
}

void ALobbyGameMode::StartGame(EPlayMode PlayMode)
{
	if (!HasAuthority()) return;
	
	bUseSeamlessTravel = true;

	FString MapName = (PlayMode == EPlayMode::BattleRoyale) ? TEXT("Lvl_SnakeBattle1") : TEXT("Lvl_Coop1");
    
	// ServerTravel handles moving ALL connected clients
	GetWorld()->ServerTravel(MapName + TEXT("?listen"), true);
}