// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverUserWidget.generated.h"

class UButton;
class UTextBlock;
class FText;

UCLASS()
class UGameOverUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelEndedTextBox;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player1ScoreTextBox;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player2ScoreTextBox;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player3ScoreTextBox;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player4ScoreTextBox;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalScoreTextBox;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Button_NextLevel;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Button_ToMainMenu;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Button_QuitGame;
	
	UFUNCTION()
	void OnNextLevelClicked();
	
	UFUNCTION()
	void OnToMainMenuClicked();
	
	UFUNCTION()
	void OnQuitClicked();
	
	UFUNCTION()
	void UpdateLevelEndedDisplay(FString& NewText);
};
