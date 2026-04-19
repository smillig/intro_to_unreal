// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyUserWidget.h"
#include "LobbyGameMode.generated.h"

UCLASS()
class ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ALobbyGameMode();
	
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<ULobbyUserWidget> ULobbyUserWidget;
	
	UFUNCTION(BlueprintCallable)
	void StartGame(EPlayMode PlayMode);
	
	virtual void PostLogin(APlayerController* NewPlayerController) override;
	
protected:
	virtual void BeginPlay() override;		
	
private:
	// A map to store names tied to the player's unique ID until they are ready
	UPROPERTY()
	TMap<APlayerController*, FString> PendingNames;	
};
