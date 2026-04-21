// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SnakeGameMode.h"
#include "BattleSnakeGameMode.generated.h"


UCLASS()
class ABattleSnakeGameMode : public ASnakeGameMode
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TMap<APlayerController*, FString> PendingNames;
	
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
public:
	ABattleSnakeGameMode();
	
	// We override this to spawn a snake for every player that travels in
	virtual void PostLogin(APlayerController* NewPlayerController) override;
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	// Helper function so we don't repeat logic
	// void SpawnSnakeForPlayer(APlayerController* PC);

	int32 NextStartIndex;
	
	int32 PlayersSpawnedCount = 0;
};
