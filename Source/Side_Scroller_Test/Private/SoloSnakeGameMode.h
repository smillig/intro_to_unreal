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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemSpawnCountLevel1 = 35;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemSpawnCountLevel2 = 20;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemSpawnCountLevel3 = 15;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerMovementAdjustment1 = -0.05f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerMovementAdjustment2 = -0.1f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerMovementAdjustment3 = -0.15f;
};
