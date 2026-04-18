// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyUserWidget.h"
#include "SnakePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SIDE_SCROLLER_TEST_API ASnakePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Client, Reliable)
	void Client_ShowLobbyUI(TSubclassOf<ULobbyUserWidget> WidgetClass);
};
