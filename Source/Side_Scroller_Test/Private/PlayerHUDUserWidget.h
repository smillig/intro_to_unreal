// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDUserWidget.generated.h"

class UTextBlock;

UCLASS()
class UPlayerHUDUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreTextBox;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimerTextBox;
	
	UFUNCTION()
	void UpdateScoreDisplay(int32 NewScore);
	
	UFUNCTION()
	void UpdateTimerDisplay(int32 TimeRemaining);
	
private:
	// loop timer to avoid network timing issues
	void TryBindDelegates();
	
	FTimerHandle InitTimerHandle;
};
