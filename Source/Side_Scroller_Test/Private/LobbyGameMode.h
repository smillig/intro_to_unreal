// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"


UCLASS()
class ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> ULobbyUserWidget;
	
	UFUNCTION(BlueprintCallable)
	void StartGame(EPlayMode PlayMode);
	
	virtual void PostLogin(APlayerController* NewPlayerController) override;
	
protected:
	virtual void BeginPlay() override;		
};
