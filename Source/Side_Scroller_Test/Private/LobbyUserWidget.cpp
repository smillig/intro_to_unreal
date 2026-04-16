// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "LobbyUserWidget.h"

void ULobbyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Button_LeaveLobby) Button_LeaveLobby->OnClicked.AddDynamic(this, &ULobbyUserWidget::OnLeaveLobbyClicked);
	// ... existing logic ...
}

void ULobbyUserWidget::OnLeaveLobbyClicked()
{
	// This opens the Main Menu level, effectively disconnecting the player from the server
	UGameplayStatics::OpenLevel(GetWorld(), FName("L_MainMenu"));
}


