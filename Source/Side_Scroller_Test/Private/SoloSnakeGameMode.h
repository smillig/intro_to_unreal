// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SnakeGameMode.h"
#include "SoloSnakeGameMode.generated.h"

UCLASS()
class ASoloSnakeGameMode : public ASnakeGameMode
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
public:
	ASoloSnakeGameMode();
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
};
