// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUserWidget.generated.h"

class UWidgetSwitcher;
class UButton;
class UEditableText;

UCLASS()
class UMainMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// THE SWITCHER: In UMG, name your WidgetSwitcher "MenuSwitcher"
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	// --- PAGE 0: MAIN ---
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Solo;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Multiplayer; // Opens the Hallway
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Quit;

	// --- PAGE 1: MULTIPLAYER HALLWAY ---
	UPROPERTY(meta = (BindWidget))
	UButton* Button_ToHostPage;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_ToJoinPage;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_BackToMain;

	// --- PAGE 2: HOST SETTINGS ---
	UPROPERTY(meta = (BindWidget))
	UEditableText* Host_IPDisplay;
	UPROPERTY(meta = (BindWidget))
	UEditableText* Host_NameInput;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_FinalHost;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_BackToHallway_FromHost;

	// --- PAGE 3: JOIN SETTINGS ---
	UPROPERTY(meta = (BindWidget))
	UEditableText* Join_NameInput;
	UPROPERTY(meta = (BindWidget))
	UEditableText* Join_IPInput;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_FinalJoin;
	UPROPERTY(meta = (BindWidget))
	UButton* Button_BackToHallway_FromJoin;

protected:
	virtual void NativeConstruct() override;

	// Navigation
	UFUNCTION() void OnMultiplayerClicked();
	UFUNCTION() void OnBackToMainClicked();
	UFUNCTION() void OnBackToHallwayClicked();
	UFUNCTION() void OnToHostPageClicked();
	UFUNCTION() void OnToJoinPageClicked();

	// Actions
	UFUNCTION() void OnSoloClicked();
	UFUNCTION() void OnFinalHostClicked();
	UFUNCTION() void OnFinalJoinClicked();
	UFUNCTION() void OnQuitClicked();
	
	// Networking
	UFUNCTION() FString GetLocalIPAddress();
};
