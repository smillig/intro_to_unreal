// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SnakeGameMode.h"
#include "CoopSnakeGameMode.generated.h"


UCLASS()
class ACoopSnakeGameMode : public ASnakeGameMode
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	TMap<APlayerController*, FString> PendingNames;
	
	// virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
	
	virtual void BeginPlay() override;
	
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemSpawnCountLevel1 = 15;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemSpawnCountLevel2 = 10;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemSpawnCountLevel3 = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerMovementAdjustment1 = -0.1f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerMovementAdjustment2 = -0.25f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerMovementAdjustment3 = -0.4f;
	
public:
	ACoopSnakeGameMode();
	
	// We override this to spawn a snake for every player that travels in
	virtual void PostLogin(APlayerController* NewPlayerController) override;
	
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual void HandleSeamlessTravelPlayer(AController*& Contr) override;

private:
	UPROPERTY()
	TMap<APlayerController*, int32> PendingSlots;
	
	int32 NextSlotID = 0;
};
