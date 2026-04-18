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
	bool bIsHost = GetOwningPlayer()->HasAuthority();
	
	USnakeGameInstance* GI = Cast<USnakeGameInstance>(GetGameInstance());
	if (!GI) return;
	
	ASnakeGameState* GameState = Cast<ASnakeGameState>(GetWorld()->GetGameState());
	if (!GameState) return;
	
	if (Button_StartBattle && Button_StartCoop)
	{
		// Only the server (Host) should see the "Start" buttons
		bool bIsServer = GetWorld()->GetNetMode() < NM_Client; 
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

	if (ASnakeGameState* GS = GetWorld()->GetGameState<ASnakeGameState>())
	{
		// PlayerArray contains all connected players' PlayerStates
		if (GS->PlayerArray.Num() >= 1)
		{
			if (ASnakePlayerState* PS = Cast<ASnakePlayerState>(GS->PlayerArray[0]))
			{
				if (Host_NameDisplay) Host_NameDisplay->SetText(FText::FromString(PS->SnakeName));
			}
		}
		if (GS->PlayerArray.Num() >= 2)
		{
			if (ASnakePlayerState* PS = Cast<ASnakePlayerState>(GS->PlayerArray[1]))
			{
				if (Client_NameDisplay) Client_NameDisplay->SetText(FText::FromString(PS->SnakeName));
			}
		}
	}
}

void ULobbyUserWidget::OnLeaveLobbyClicked()
{
	// This opens the Main Menu level, effectively disconnecting the player from the server
	UGameplayStatics::OpenLevel(GetWorld(), FName("Lvl_MainMenu"));
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
