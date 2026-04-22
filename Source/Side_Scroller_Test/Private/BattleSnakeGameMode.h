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
	
	// virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
	
	virtual void BeginPlay() override;
	
public:
	ABattleSnakeGameMode();
	
	// We override this to spawn a snake for every player that travels in
	virtual void PostLogin(APlayerController* NewPlayerController) override;
	
	// virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual void HandleSeamlessTravelPlayer(AController*& Contr) override;
	
private:
	FTimerHandle SpawnTimerHandle;

	int32 NextStartIndex = 0;
	
	int32 PlayersSpawnedCount = 0;
	
	int32 NextSlotID = 0;
	
	UPROPERTY()
	TArray<AController*> ReadyPlayers;
	
	UPROPERTY(EditDefaultsOnly, Category="Match")
	int32 ExpectedPlayerCount = 2;
};
