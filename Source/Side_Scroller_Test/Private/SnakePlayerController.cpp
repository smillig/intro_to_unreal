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
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(LobbyWidget->TakeWidget());
			SetInputMode(InputMode);
		}
	}
}

void ASnakePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Warning, TEXT("OnPossess: %s"), *GetNameSafe(InPawn));

	SetViewTargetWithBlend(InPawn);

	// Restore game input here (important after lobby)
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
}

void ASnakePlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	UE_LOG(LogTemp, Warning, TEXT("OnRep_Pawn: %s"), *GetNameSafe(GetPawn()));

	if (GetPawn())
	{
		SetViewTargetWithBlend(GetPawn());
	}
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