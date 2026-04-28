// Fill out your copyright notice in the Description page of Project Settings.


#include "SoloSnakeGameMode.h"
#include "SnakePlayerState.h"
#include "SnakePlayerController.h"
#include "SoloSnakeGameState.h"
#include "EngineUtils.h"
#include "SnakeGameState.h"
#include "GameFramework/PlayerStart.h"

ASoloSnakeGameMode::ASoloSnakeGameMode()
{
	// Point this GameMode to use your specific Battle classes
	GameStateClass = ASoloSnakeGameState::StaticClass();
	PlayerStateClass = ASnakePlayerState::StaticClass();
	PlayerControllerClass = ASnakePlayerController::StaticClass();
	
	bUseSeamlessTravel = false;
}

void ASoloSnakeGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	ASoloSnakeGameState* GotGameState = GetGameState<ASoloSnakeGameState>();
	if (GotGameState)
	{
		int32 CurrentLevel = GotGameState->CurrentLevel;
		switch (CurrentLevel)
		{
		case 1:
			{
				MaxFoodOnBoard = GotGameState->ItemSpawnCountLevel1;
				// GotGameState->CurrentMovementAdjustment = GotGameState->PlayerMovementAdjustment1;
				// UE_LOG(LogTemp, Warning, TEXT("CurrentMovementAdjustment: %f"), GotGameState->CurrentMovementAdjustment);
				break;
			}
		case 2:
			{
				MaxFoodOnBoard = GotGameState->ItemSpawnCountLevel2; 
				// CurrentMovementAdjustment = PlayerMovementAdjustment2;
				break;
			}
		case 3:
			{
				MaxFoodOnBoard = GotGameState->ItemSpawnCountLevel3;
				// CurrentMovementAdjustment = PlayerMovementAdjustment3;
				break;
			}
		default:
			{
				MaxFoodOnBoard = GotGameState->ItemSpawnCountLevel1;
				// CurrentMovementAdjustment = PlayerMovementAdjustment1;
				break;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("MaxFoodOnBoard: %d"), MaxFoodOnBoard);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MaxFoodOnBoard not set"));
	}
}

AActor* ASoloSnakeGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// 1. Define the tag we are looking for
	// (Ensure this matches exactly what is typed in the UI, e.g., "Start0")
	FName TargetTag = TEXT("Start0"); 

	// 2. Iterate through ALL PlayerStarts in the level
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* PStart = *It;
        
		// 3. Check the specific PlayerStartTag variable
		if (PStart && PStart->PlayerStartTag == TargetTag)
		{
			UE_LOG(LogTemp, Log, TEXT("Successfully found PlayerStart with PlayerStartTag: %s"), *TargetTag.ToString());
			return PStart;
		}
	}

	// 4. Fallback if the tag wasn't found
	UE_LOG(LogTemp, Warning, TEXT("Could not find PlayerStart with tag %s! Using default."), *TargetTag.ToString());
	return Super::ChoosePlayerStart_Implementation(Player);
}
