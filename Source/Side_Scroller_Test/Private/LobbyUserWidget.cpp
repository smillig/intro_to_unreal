// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "SocketSubsystem.h"

void ULobbyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Host_IPDisplay)
	{
		FString LocalIP = GetLocalIPAddress();
		Host_IPDisplay->SetText(FText::FromString(LocalIP));
	}
	
	if (Button_LeaveLobby) Button_LeaveLobby->OnClicked.AddDynamic(this, &ULobbyUserWidget::OnLeaveLobbyClicked);
	if (Button_StartBattle) Button_StartBattle->OnClicked.AddDynamic(this, &ULobbyUserWidget::OnStartBattleClicked);
	if (Button_StartCoop) Button_StartCoop->OnClicked.AddDynamic(this, &ULobbyUserWidget::OnStartCoopClicked);
}

void ULobbyUserWidget::OnLeaveLobbyClicked()
{
	// This opens the Main Menu level, effectively disconnecting the player from the server
	UGameplayStatics::OpenLevel(GetWorld(), FName("Lvl_MainMenu"));
}

void ULobbyUserWidget::OnStartBattleClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("Lvl_SnakeBattle1"));
}

void ULobbyUserWidget::OnStartCoopClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("Lvl_Coop1"));
}

FString ULobbyUserWidget::GetLocalIPAddress()
{
	// Get the local IP address
	bool bCanBind = false;
	TSharedRef<FInternetAddr> LocalIP = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBind);
	
	if (LocalIP->IsValid())
	{
		return LocalIP->ToString(true); 
	}
	
	return FString("127.0.0.1"); // Fallback to localhost
}