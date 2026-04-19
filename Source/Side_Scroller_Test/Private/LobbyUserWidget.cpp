// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "SnakeGameInstance.h"
#include "SnakePlayerState.h"
#include "SnakePlayerController.h"
#include "LobbyGameMode.h"
#include "SnakeGameState.h"

void ULobbyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetIsFocusable(true);
	
	if (Button_LeaveLobby) Button_LeaveLobby->OnClicked.AddDynamic(this, &ULobbyUserWidget::OnLeaveLobbyClicked);
    if (Button_StartBattle) Button_StartBattle->OnClicked.AddDynamic(this, &ULobbyUserWidget::OnStartBattleClicked);
    if (Button_StartCoop) Button_StartCoop->OnClicked.AddDynamic(this, &ULobbyUserWidget::OnStartCoopClicked);
	
	APlayerController* PC = GetOwningPlayer();
	if (Button_StartBattle && Button_StartCoop && PC)
	{
		bool bIsHost = PC->HasAuthority();
		// Only the server (Host) should see the "Start" buttons
		Button_StartBattle->SetVisibility(bIsHost ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		Button_StartCoop->SetVisibility(bIsHost ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	USnakeGameInstance* GI = Cast<USnakeGameInstance>(GetGameInstance());
	if (!GI) return;
	
	ASnakeGameState* GameState = Cast<ASnakeGameState>(GetWorld()->GetGameState());
	if (!GameState) return;
	
	// // Display host name
	// if (Host_NameDisplay)
	// {
	// 	Host_NameDisplay->SetText(FText::FromString(*GI->UserPlayerName));
	// }
	//
	// // Display client name (or "Waiting for player..." if empty)
	// if (Client_NameDisplay)
	// {
	// 	FString ClientName = GI->UserPlayerName.IsEmpty() ? "Waiting for player..." : GameState->ClientPlayerName;
	// 	Client_NameDisplay->SetText(FText::FromString(ClientName));
	// }
	
	if (Host_IPDisplay)
	{
		FString IP_Port = FString::Printf(TEXT("%s:%s"), *GI->HostIPAddress, *GI->HostPort);
		Host_IPDisplay->SetText(FText::FromString(IP_Port));
		Host_IPDisplay->SetIsReadOnly(true);
	}
	
	
}

void ULobbyUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	ASnakeGameState* GS = GetWorld()->GetGameState<ASnakeGameState>();
	if (!GS) return;
	
	// display IP address of host and keep it updated
	if (Host_IPDisplay)
	{
		Host_IPDisplay->SetText(FText::FromString(GS->ServerDisplayIP));
	}
	
	// Reset to default
	if (Host_NameDisplay) Host_NameDisplay->SetText(FText::FromString(TEXT("Waiting...")));
	if (Client_NameDisplay) Client_NameDisplay->SetText(FText::FromString(TEXT("Waiting...")));

	// Loop through all players currently in the GameState
	for (APlayerState* BasePS : GS->PlayerArray)
	{
		ASnakePlayerState* PS = Cast<ASnakePlayerState>(BasePS);
		if (!PS || PS->SnakeName.IsEmpty()) continue;

		// UNREAL'S SECRET WEAPON: IsPrimaryPlayer() 
		// On a Listen Server, the Host is ALWAYS the first player.
		// Alternatively, we can use the PlayerID.
		
		// We'll use a simpler logic: 
		// If the PlayerState's index in the array is 0, they are the Host.
		int32 PlayerIndex = GS->PlayerArray.Find(BasePS);

		if (PlayerIndex == 0 && Host_NameDisplay)
		{
			Host_NameDisplay->SetText(FText::FromString(PS->SnakeName));
		}
		else if (PlayerIndex == 1 && Client_NameDisplay)
		{
			Client_NameDisplay->SetText(FText::FromString(PS->SnakeName));
		}
	}
}

void ULobbyUserWidget::OnLeaveLobbyClicked()
{
	// log button push
	// UE_LOG(LogTemp, Warning, TEXT("Leave Lobby Button Clicked."));
	
	
	ASnakePlayerController* PC = Cast<ASnakePlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->Server_LeaveLobby();
		// For a client, "Leaving" means traveling back to their own local Main Menu
		// This automatically disconnects them from the server.
		PC->ClientTravel(TEXT("/Game/Snake/Levels/Lvl_MainMenu"), ETravelType::TRAVEL_Absolute);
	}
}

void ULobbyUserWidget::OnStartBattleClicked()
{
	ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>();
	if (GM) GM->StartGame(EPlayMode::BattleRoyale);
	UGameplayStatics::OpenLevel(GetWorld(), FName("Lvl_SnakeBattle1"));
}

void ULobbyUserWidget::OnStartCoopClicked()
{
	ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>();
	if (GM) GM->StartGame(EPlayMode::CoOp);
	UGameplayStatics::OpenLevel(GetWorld(), FName("Lvl_Coop1"));
}
