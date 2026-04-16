// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUserWidget.generated.h"

class UButton;

UCLASS()
class ULobbyUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// Added override declaration
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Button_LeaveLobby;

public:
	
	UFUNCTION()
	void OnLeaveLobbyClicked();
};
