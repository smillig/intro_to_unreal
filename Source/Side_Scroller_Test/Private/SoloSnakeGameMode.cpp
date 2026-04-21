// Fill out your copyright notice in the Description page of Project Settings.


#include "SoloSnakeGameMode.h"
#include "SnakePlayerState.h"
#include "SnakePlayerController.h"
#include "EngineUtils.h"
#include "SnakeGameState.h"
#include "GameFramework/PlayerStart.h"

ASoloSnakeGameMode::ASoloSnakeGameMode()
{
	// Point this GameMode to use your specific Battle classes
	GameStateClass = ASnakeGameState::StaticClass();
	PlayerStateClass = ASnakePlayerState::StaticClass();
	PlayerControllerClass = ASnakePlayerController::StaticClass();
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
