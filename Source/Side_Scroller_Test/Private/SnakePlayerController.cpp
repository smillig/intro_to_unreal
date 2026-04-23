// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePlayerController.h"
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
	
}

void ASnakePlayerController::Server_LeaveLobby_Implementation()
{
	if (!IsLocalController())
	{
		// GameMode->Logout(this);
		// ClientTravel(TEXT("/Game/Maps/MainMenu"), ETravelType::TRAVEL_Absolute);
		Destroy();
	}
}