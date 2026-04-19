// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"

void ASnakePlayerController::Client_ShowLobbyUI_Implementation(TSubclassOf<ULobbyUserWidget> WidgetClass)
{
	// THIS CODE RUNS ON THE CLIENT'S MACHINE
	if (WidgetClass && IsLocalController())
	{
		UUserWidget* LobbyWidget = CreateWidget<UUserWidget>(this, WidgetClass);
		if (LobbyWidget)
		{
			LobbyWidget->AddToViewport();

			bShowMouseCursor = true;
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(LobbyWidget->TakeWidget());
			SetInputMode(InputMode);
		}
	}
}

void ASnakePlayerController::Server_LeaveLobby_Implementation()
{
	// This tells the server to destroy this controller and its PlayerState immediately
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		GetWorld()->GetAuthGameMode()->Logout(this);
	}
    
	// Destroy the controller on the server
	Destroy();
}