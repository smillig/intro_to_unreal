// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuUserWidget.generated.h"

class UButton;

UCLASS()
class UPauseMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(meta = (BindWidget))
	UButton* Button_ResumeGame;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Button_ToMainMenu;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Button_QuitGame;
};
