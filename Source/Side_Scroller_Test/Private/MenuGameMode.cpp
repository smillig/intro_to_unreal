// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UMainMenuUserWidget)
	{
		UUserWidget* Menu = CreateWidget<UUserWidget>(GetWorld(), UMainMenuUserWidget);
		if (Menu) Menu->AddToViewport();
		// Optional: show mouse cursor
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeUIOnly());
		}
	}
}
