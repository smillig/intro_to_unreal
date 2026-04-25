// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyUserWidget.h"
#include "PlayerHUDUserWidget.h"
#include "PauseMenuUserWidget.h"
#include "GameOverUserWidget.h"
#include "SnakePlayerController.generated.h"


UCLASS()
class SIDE_SCROLLER_TEST_API ASnakePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPauseMenuUserWidget> PauseMenuWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameOverUserWidget> GameOverWidgetClass;
	
	UFUNCTION(Client, Reliable)
	void Client_ShowLobbyUI(TSubclassOf<ULobbyUserWidget> WidgetClass);
	
	UFUNCTION(Client, Reliable)
	void Client_TogglePauseMenu(bool bIsPaused);
	
	UFUNCTION(Server, Reliable)
	void Server_LeaveLobby();
	
	virtual void OnRep_Pawn() override;
	
	UFUNCTION(Client, Reliable)
	void Client_ShowGameOverScreen();
	
	UFUNCTION(Client, Reliable)
	void Client_ShowPlayerHud(TSubclassOf<UPlayerHUDUserWidget> PlayerWidgetClass);

protected:
	virtual void OnPossess(APawn* Pawn) override;
	
private:
	UPROPERTY()
	UUserWidget* PauseMenuWidget;
	
	UPROPERTY()
	UUserWidget* GameOverWidget;
};
