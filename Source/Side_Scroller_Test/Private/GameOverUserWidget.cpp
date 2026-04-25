// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "SnakePlayerState.h"
#include "SnakeGameState.h"

void UGameOverUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// bind buttons
	if (Button_NextLevel) Button_NextLevel->OnClicked.AddDynamic(this, &UGameOverUserWidget::OnNextLevelClicked);
	if (Button_ToMainMenu) Button_ToMainMenu->OnClicked.AddDynamic(this, &UGameOverUserWidget::OnToMainMenuClicked);
	if (Button_QuitGame) Button_QuitGame->OnClicked.AddDynamic(this, &UGameOverUserWidget::OnQuitClicked);
	
	
	// Only the Host should be allowed to click "Next Level"
	if (GetOwningPlayer() && !GetOwningPlayer()->HasAuthority())
	{
		if (Button_NextLevel) Button_NextLevel->SetVisibility(ESlateVisibility::Collapsed);
	}

	ASnakeGameState* GS = GetWorld()->GetGameState<ASnakeGameState>();
	if (!GS) return;
	
	if (GS->CurrentLevel > 2)
	{
		if (Button_NextLevel) Button_NextLevel->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	// Change the tile 
	FString LevelTitle;
	if (GS->CurrentLevel < 3)
	{
		switch (GS->CurrentPlayMode)
		{
			case EPlayMode::Solo: 
				LevelTitle = FString::Printf(TEXT("Level %d Finished!"), GS->CurrentLevel); break;
			case EPlayMode::BattleRoyale: 
				LevelTitle = FString::Printf(TEXT("Battle Royale Level %d Finished!"), GS->CurrentLevel); break;
			case EPlayMode::CoOp: 
				LevelTitle = FString::Printf(TEXT("CoOp Level %d Finished!"), GS->CurrentLevel); break;
			default: 
				LevelTitle = FString::Printf(TEXT("Unknown Level %d Finished!"), GS->CurrentLevel); break;
		}
	}
	else
	{
		LevelTitle = TEXT("GAME OVER");
	}
	UpdateLevelEndedDisplay(LevelTitle);

	// Button visibility according to how many players
	
	// Hide all individual scores by default
	if (Player1ScoreTextBox) Player1ScoreTextBox->SetVisibility(ESlateVisibility::Collapsed);
	if (Player2ScoreTextBox) Player2ScoreTextBox->SetVisibility(ESlateVisibility::Collapsed);
	if (Player3ScoreTextBox) Player3ScoreTextBox->SetVisibility(ESlateVisibility::Collapsed);
	if (Player4ScoreTextBox) Player4ScoreTextBox->SetVisibility(ESlateVisibility::Collapsed);
	if (TotalScoreTextBox) TotalScoreTextBox->SetVisibility(ESlateVisibility::Collapsed);

	int32 CombinedScore = 0;

	// Loop through all connected players
	for (int32 i = 0; i < GS->PlayerArray.Num(); i++)
	{
		ASnakePlayerState* PS = Cast<ASnakePlayerState>(GS->PlayerArray[i]);
		if (!PS) continue;

		CombinedScore += PS->SnakeScore;

		FString ScoreString = FString::Printf(TEXT("%s: %d"), *PS->SnakeName, PS->SnakeScore);

		// Assign them to a text box based on their loop index
		if (i == 0 && Player1ScoreTextBox) 
		{
			Player1ScoreTextBox->SetText(FText::FromString(ScoreString));
			Player1ScoreTextBox->SetVisibility(ESlateVisibility::Visible);
		}
		else if (i == 1 && Player2ScoreTextBox)
		{
			Player2ScoreTextBox->SetText(FText::FromString(ScoreString));
			Player2ScoreTextBox->SetVisibility(ESlateVisibility::Visible);
		}
		else if (i == 2 && Player3ScoreTextBox)
		{
			Player3ScoreTextBox->SetText(FText::FromString(ScoreString));
			Player3ScoreTextBox->SetVisibility(ESlateVisibility::Visible);
		}
		else if (i == 3 && Player4ScoreTextBox)
		{
			Player4ScoreTextBox->SetText(FText::FromString(ScoreString));
			Player4ScoreTextBox->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// Show Total Score if it's Co-Op
	if (GS->CurrentPlayMode == EPlayMode::CoOp && TotalScoreTextBox)
	{
		FString TotalStr = FString::Printf(TEXT("Combined Team Score: %d"), CombinedScore);
		TotalScoreTextBox->SetText(FText::FromString(TotalStr));
		TotalScoreTextBox->SetVisibility(ESlateVisibility::Visible);
	}
}

void UGameOverUserWidget::OnNextLevelClicked()
{
	// Ensure only the server can click this
	APlayerController* PC = GetOwningPlayer();
	if (PC && PC->HasAuthority())
	{
		ASnakeGameState* GS = GetWorld()->GetGameState<ASnakeGameState>();
		if (GS)
		{
			int32 NextLevelNum = GS->CurrentLevel + 1;
			FString NextMapName = FString::Printf(TEXT("/Game/Snake/Levels/Lvl_MainMenu"));
			switch (GS->CurrentPlayMode)
			{
			case EPlayMode::Solo:
				{
					NextMapName = FString::Printf(TEXT("Lvl_Solo%d?listen"), NextLevelNum);
					break;	
				}
			case EPlayMode::BattleRoyale:
				{
					NextMapName = FString::Printf(TEXT("Lvl_SnakeBattle%d?listen"), NextLevelNum);
					break;
				}
			case EPlayMode::CoOp:
				{
					NextMapName = FString::Printf(TEXT("Lvl_Coop%d?listen"), NextLevelNum);
					break;
				}
			default:
				{
					UE_LOG(LogTemp, Error, TEXT("Unknown PlayMode"));
					NextMapName = FString::Printf(TEXT("/Game/Snake/Levels/Lvl_MainMenu"));
					break;
				}
			}
			GS->CurrentLevel++;
			GetWorld()->ServerTravel(NextMapName, true);
		}
	}
}

void UGameOverUserWidget::OnToMainMenuClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		if (PC->HasAuthority())
		{
			// Server closes lobby, kicks everyone to main menu
			GetWorld()->ServerTravel(TEXT("/Game/Maps/Lvl_MainMenu"));
		}
		else
		{
			// Client disconnects and returns to their own main menu
			PC->ClientTravel(TEXT("/Game/Maps/Lvl_MainMenu"), ETravelType::TRAVEL_Absolute);
		}
	}
}

void UGameOverUserWidget::OnQuitClicked()
{
	APlayerController* PC = GetOwningPlayer();
	UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, true);
}

void UGameOverUserWidget::UpdateLevelEndedDisplay(FString& NewText)
{
	if (LevelEndedTextBox) LevelEndedTextBox->SetText(FText::FromString(NewText));
}
