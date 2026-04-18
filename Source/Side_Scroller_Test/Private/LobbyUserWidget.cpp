// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "SnakeGameInstance.h"
#include "SnakePlayerState.h"
#include "LobbyGameMode.h"
#include "SnakeGameState.h"

void ULobbyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Check if we are the Host (Server)
	bool bIsServer = (GetWorld()->GetAuthGameMode() != nullptr);
	
	USnakeGameInstance* GI = Cast<USnakeGameInstance>(GetGameInstance());
	if (!GI) return;
	
	ASnakeGameState* GameState = Cast<ASnakeGameState>(GetWorld()->GetGameState());
	if (!GameState) return;
	
	SetIsFocusable(true);
	
	if (Button_StartBattle && Button_StartCoop)
	{
		// Only the server (Host) should see the "Start" buttons
		Button_StartBattle->SetVisibility(bIsServer ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		Button_StartCoop->SetVisibility(bIsServer ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	
	// Display host name
	if (Host_NameDisplay)
	{
		Host_NameDisplay->SetText(FText::FromString(*GI->UserPlayerName));
	}
	
	// Display client name (or "Waiting for player..." if empty)
	if (Client_NameDisplay)
	{
		FString ClientName = GI->UserPlayerName.IsEmpty() ? "Waiting for player..." : GameState->ClientPlayerName;
		Client_NameDisplay->SetText(FText::FromString(ClientName));
	}
	
	if (Host_IPDisplay)
	{
		FString IP_Port = FString::Printf(TEXT("%s:%s"), *GI->HostIPAddress, *GI->HostPort);
		Host_IPDisplay->SetText(FText::FromString(IP_Port));
		Host_IPDisplay->SetIsReadOnly(true);
	}
	
	if (Button_LeaveLobby) Button_LeaveLobby->OnClicked.AddDynamic(this, &ULobbyUserWidget::OnLeaveLobbyClicked);
	if (Button_StartBattle) Button_StartBattle->OnClicked.AddDynamic(this, &ULobbyUserWidget::OnStartBattleClicked);
	if (Button_StartCoop) Button_StartCoop->OnClicked.AddDynamic(this, &ULobbyUserWidget::OnStartCoopClicked);
}

void ULobbyUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	ASnakeGameState* GS = GetWorld()->GetGameState<ASnakeGameState>();
	if (!GS) return;

	if (Host_IPDisplay)
	{
		Host_IPDisplay->SetText(FText::FromString(GS->ServerDisplayIP));
	}
	
	// Loop through all connected players
	for (int32 i = 0; i < GS->PlayerArray.Num(); i++)
	{
		ASnakePlayerState* PS = Cast<ASnakePlayerState>(GS->PlayerArray[i]);
		if (PS)
		{
			// Player 0 is usually the Host
			if (i == 0 && Host_NameDisplay)
			{
				Host_NameDisplay->SetText(FText::FromString(PS->SnakeName));
			}
			// Player 1 is the first Client
			else if (i == 1 && Client_NameDisplay)
			{
				Client_NameDisplay->SetText(FText::FromString(PS->SnakeName));
			}
		}
	}
}

void ULobbyUserWidget::OnLeaveLobbyClicked()
{
	// log button push
	UE_LOG(LogTemp, Warning, TEXT("Leave Lobby Button Clicked."));
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
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
