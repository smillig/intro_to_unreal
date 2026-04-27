// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenuUserWidget.h"
#include "SnakePawn.h"
#include "SnakePlayerController.h"
#include "SnakeGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

void UPauseMenuUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetIsFocusable(true);
	
	if (Button_ResumeGame) Button_ResumeGame->OnClicked.AddDynamic(this, &UPauseMenuUserWidget::OnResumeClicked);
	if (Button_ToMainMenu) Button_ToMainMenu->OnClicked.AddDynamic(this, &UPauseMenuUserWidget::OnToMainMenuClicked);
	if (Button_QuitGame) Button_QuitGame->OnClicked.AddDynamic(this, &UPauseMenuUserWidget::OnQuitClicked);
}

void UPauseMenuUserWidget::OnResumeClicked()
{
	ASnakePawn* SnakePawn = Cast<ASnakePawn>(GetOwningPlayerPawn());
	SnakePawn->Server_RequestPause();
}

void UPauseMenuUserWidget::OnToMainMenuClicked()
{
	ASnakePlayerController* PC = Cast<ASnakePlayerController>(GetOwningPlayer());
	if (!PC) return;
	USnakeGameInstance* GI = GetWorld()->GetGameInstance<USnakeGameInstance>();
	if (GI)
	{
		GI->CurrentSnakeLevel = 1;
	}
	if (PC->HasAuthority())
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("Lvl_MainMenu"), true);
	}
	else
	{
		PC->Server_LeaveLobby();
		PC->ClientTravel(TEXT("/Game/Snake/Levels/Lvl_MainMenu"), ETravelType::TRAVEL_Absolute);
	}
}

void UPauseMenuUserWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}