// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "EngineUtils.h"
#include "PlayerHUDUserWidget.h"
#include "GameFramework/GameMode.h"

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
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(LobbyWidget->TakeWidget());
			SetInputMode(InputMode);
		}
		else
		{
			FInputModeGameOnly InputMode;
			SetInputMode(InputMode);
			bShowMouseCursor = false;
		}
	}
}

void ASnakePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Warning, TEXT("Controller %s possessed %s"),
		*GetName(),
		*InPawn->GetName());

	if (IsLocalController() && InPawn)
	{
		SetViewTargetWithBlend(InPawn);
	}
	
}

void ASnakePlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	UE_LOG(LogTemp, Warning, TEXT("OnRep_Pawn: %s"), *GetNameSafe(GetPawn()));

	if (IsLocalController() && GetPawn())
	{
		SetViewTargetWithBlend(GetPawn());
	}
}

void ASnakePlayerController::Client_ShowGameOverScreen_Implementation()
{
	// UE_LOG(LogTemp, Warning, TEXT("Client_ShowGameOverScreen called"))
	if (GameOverWidgetClass && IsLocalController())
	{
		// find and disable Hud widget
		if (PlayerHUDWidget && PlayerHUDWidget->IsInViewport())
		{
			PlayerHUDWidget->RemoveFromParent();
		}
		
		GameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();
			
			bShowMouseCursor = true;
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(GameOverWidget->TakeWidget());
			SetInputMode(InputMode);
		}
	}
}

void ASnakePlayerController::Client_ShowPlayerHud_Implementation(TSubclassOf<UPlayerHUDUserWidget> PlayerWidgetClass)
{
	// This only fires when the Battle/Coop GameMode explicitly asks for it
	if (PlayerWidgetClass && IsLocalController())
	{
		PlayerHUDWidget = CreateWidget<UUserWidget>(this, PlayerWidgetClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
			
			// Ensure Input is set up for gameplay
			FInputModeGameOnly InputMode;
			SetInputMode(InputMode);
			bShowMouseCursor = false;
		}
	}
}

void ASnakePlayerController::Server_LeaveLobby_Implementation()
{
	if (!IsLocalController())
	{
		AGameModeBase* CurrGameMode = GetWorld()->GetAuthGameMode();
		if (CurrGameMode)
		{
			CurrGameMode->Logout(this);
		}
		// ClientTravel(TEXT("/Game/Maps/MainMenu"), ETravelType::TRAVEL_Absolute);
		// Destroy();
	}
}

void ASnakePlayerController::Client_TogglePauseMenu_Implementation(bool bIsPaused)
{
	
	if (!IsLocalController() || !PauseMenuWidgetClass) return;
	UE_LOG(LogTemp, Warning, TEXT("Client_TogglePauseMenu called, local controller and widget are good."))
	if (bIsPaused)
	{
		if (!PauseMenuWidget)
		{
			PauseMenuWidget = CreateWidget<UUserWidget>(this, PauseMenuWidgetClass);
		}
		
		if (PauseMenuWidget && !PauseMenuWidget->IsInViewport())
		{
			PauseMenuWidget->AddToViewport();
		}
		
		bShowMouseCursor = true;
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
		SetInputMode(InputMode);
	}
	else
	{
		if (PauseMenuWidget && PauseMenuWidget->IsInViewport())
		{
			PauseMenuWidget->RemoveFromParent();
		}
		
		bShowMouseCursor = false;
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}
