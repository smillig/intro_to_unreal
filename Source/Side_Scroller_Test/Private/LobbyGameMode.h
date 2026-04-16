// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"


UCLASS()
class ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void StartGame(EPlayMode PlayMode);
	
	virtual void PostLogin(APlayerController* NewPlayerController) override;
	
};
