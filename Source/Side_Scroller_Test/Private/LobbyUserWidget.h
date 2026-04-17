// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUserWidget.generated.h"

class UButton;
class UEditableText;

UCLASS()
class ULobbyUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UEditableText* Host_IPDisplay;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Button_LeaveLobby;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Button_StartBattle;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Button_StartCoop;

protected:
	// Added override declaration
    virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnLeaveLobbyClicked();
	
	UFUNCTION()
	void OnStartBattleClicked();
	
	UFUNCTION()
	void OnStartCoopClicked();
	
	UFUNCTION()
	FString GetLocalIPAddress();
};
