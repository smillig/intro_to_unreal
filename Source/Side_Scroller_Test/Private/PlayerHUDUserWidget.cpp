// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDUserWidget.h"
#include "Components/TextBlock.h"
#include "SnakeGameState.h"
#include "SnakePlayerState.h"

void UPlayerHUDUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Set initial defaults
	if (ScoreTextBox) ScoreTextBox->SetText(FText::FromString("0"));
	if (TimerTextBox) TimerTextBox->SetText(FText::FromString("0:00"));
	
	// delegates repeatedly try every 0.2 seconds to bind
	GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &UPlayerHUDUserWidget::TryBindDelegates, 0.2f, true);
}

void UPlayerHUDUserWidget::TryBindDelegates()
{
	// check that the game state is valid
	ASnakeGameState* GS = GetWorld()->GetGameState<ASnakeGameState>();
	
	// check if the player state is yet valid
	ASnakePlayerState* PS = GetOwningPlayerState<ASnakePlayerState>();
	
	// If neither are valid try again in timer InRate (0.2 seconds)
	if (!GS || !PS) return;
	
	// items are ready and we no longer need to repeat with timer
	GetWorld()->GetTimerManager().ClearTimer(InitTimerHandle);
	
	// bind the GameState timer delegate
	GS->OnTimerUpdated.AddDynamic(this, &UPlayerHUDUserWidget::UpdateTimerDisplay);
	
	// update the timer now
	UpdateTimerDisplay(GS->MatchTimeRemaining);
	
	// bind score delegate and update the score 
	PS->OnScoreChanged.AddDynamic(this, &UPlayerHUDUserWidget::UpdateScoreDisplay);
	UpdateScoreDisplay(PS->SnakeScore);
}

void UPlayerHUDUserWidget::UpdateTimerDisplay(int32 RemainingTime)
{
	if (TimerTextBox)
	{
		int32 MinutesRemaining = RemainingTime / 60;
		int32 SecondsRemaining = RemainingTime % 60;
		
		FString TimeString = FString::Printf(TEXT("%02d:%02d"), MinutesRemaining, SecondsRemaining);
		TimerTextBox->SetText(FText::FromString(TimeString));
	}
}

void UPlayerHUDUserWidget::UpdateScoreDisplay(int32 CurrentScore)
{
	if (ScoreTextBox)
	{
		ScoreTextBox->SetText(FText::AsNumber(CurrentScore));
	}
}