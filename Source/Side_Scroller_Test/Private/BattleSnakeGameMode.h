// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SnakeGameMode.h"
#include "BattleSnakeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ABattleSnakeGameMode : public ASnakeGameMode
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	TMap<APlayerController*, FString> PendingNames;
	
public:
	ABattleSnakeGameMode();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ASnakePawn> SnakePawnClass;
    
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
	
	// We override this to spawn a snake for every player that travels in
	virtual void PostLogin(APlayerController* NewPlayerController) override;	
};
