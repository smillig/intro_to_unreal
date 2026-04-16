// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MenuGameMode.generated.h"


UCLASS()
class AMenuGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> UMainMenuUserWidget;

protected:
	virtual void BeginPlay() override;	
};
