// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuUserWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "SnakeGameInstance.h"
#include "SocketSubsystem.h"
#include "SnakeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/EditableText.h"

void UMainMenuUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Host_IPDisplay)
	{
		USnakeGameInstance* GI = Cast<USnakeGameInstance>(GetGameInstance());
		FString LocalIP = GetLocalIPAddress();
		Host_IPDisplay->SetText(FText::FromString(LocalIP));
		GI->HostIPAddress = LocalIP;
	}
	
	if (Join_IPInput)
	{
		Join_IPInput->SetText(FText::FromString("127.0.0.1"));
	}
	
	if (Host_NameInput)
	{
		Host_NameInput->SetText(FText::FromString("Player1"));
	}
	
	if (Join_NameInput)
	{
		Join_NameInput->SetText(FText::FromString("Player2"));
	}
	
	// Page 0
	if (Button_Solo) Button_Solo->OnClicked.AddDynamic(this, &UMainMenuUserWidget::OnSoloClicked);
	if (Button_Multiplayer) Button_Multiplayer->OnClicked.AddDynamic(this, &UMainMenuUserWidget::OnMultiplayerClicked);
	if (Button_Quit) Button_Quit->OnClicked.AddDynamic(this, &UMainMenuUserWidget::OnQuitClicked);

	// Page 1: Hallway
	if (Button_ToHostPage) Button_ToHostPage->OnClicked.AddDynamic(this, &UMainMenuUserWidget::OnToHostPageClicked);
	if (Button_ToJoinPage) Button_ToJoinPage->OnClicked.AddDynamic(this, &UMainMenuUserWidget::OnToJoinPageClicked);
	if (Button_BackToMain) Button_BackToMain->OnClicked.AddDynamic(this, &UMainMenuUserWidget::OnBackToMainClicked);

	// Page 2: Host
	if (Button_FinalHost) Button_FinalHost->OnClicked.AddDynamic(this, &UMainMenuUserWidget::OnFinalHostClicked);
	if (Button_BackToHallway_FromHost) Button_BackToHallway_FromHost->OnClicked.AddDynamic(this, &UMainMenuUserWidget::OnBackToHallwayClicked);

	// Page 3: Join
	if (Button_FinalJoin) Button_FinalJoin->OnClicked.AddDynamic(this, &UMainMenuUserWidget::OnFinalJoinClicked);
	if (Button_BackToHallway_FromJoin) Button_BackToHallway_FromJoin->OnClicked.AddDynamic(this, &UMainMenuUserWidget::OnBackToHallwayClicked);
}

// NAVIGATION IMPLEMENTATIONS
void UMainMenuUserWidget::OnMultiplayerClicked() { if(MenuSwitcher) MenuSwitcher->SetActiveWidgetIndex(1); }
void UMainMenuUserWidget::OnBackToMainClicked()  { if(MenuSwitcher) MenuSwitcher->SetActiveWidgetIndex(0); }
void UMainMenuUserWidget::OnBackToHallwayClicked(){ if(MenuSwitcher) MenuSwitcher->SetActiveWidgetIndex(1); }
void UMainMenuUserWidget::OnToHostPageClicked()  { if(MenuSwitcher) MenuSwitcher->SetActiveWidgetIndex(2); }
void UMainMenuUserWidget::OnToJoinPageClicked()  { if(MenuSwitcher) MenuSwitcher->SetActiveWidgetIndex(3); }

void UMainMenuUserWidget::OnSoloClicked()
{
	ASnakeGameState* GS = GetWorld()->GetGameState<ASnakeGameState>();
	if (GS) {GS->CurrentPlayMode = EPlayMode::Solo;}
	UGameplayStatics::OpenLevel(GetWorld(), FName("Lvl_Solo1"));
}

void UMainMenuUserWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}

void UMainMenuUserWidget::OnFinalHostClicked()
{
	USnakeGameInstance* GI = Cast<USnakeGameInstance>(GetGameInstance());
	if (GI && Host_NameInput)
	{
		GI->UserPlayerName = Host_NameInput->GetText().ToString();
		GI->HostGame();
	}
}

void UMainMenuUserWidget::OnFinalJoinClicked()
{
	USnakeGameInstance* GI = Cast<USnakeGameInstance>(GetGameInstance());
	if (GI && Join_NameInput && Join_IPInput)
	{
		// Save the name the user typed
		GI->UserPlayerName = Join_NameInput->GetText().ToString();
		GI->HostIPAddress = Join_IPInput->GetText().ToString();
		
		// Get IP and Port from UI
		FString IP = Join_IPInput->GetText().ToString();
		FString Port = GI->HostPort;

		// Join
		GI->JoinGame(IP, Port);
	}
}

FString UMainMenuUserWidget::GetLocalIPAddress()
{
	// Get the local IP address
	bool bCanBind = false;
	TSharedRef<FInternetAddr> LocalIP = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBind);
	
	if (LocalIP->IsValid())
	{
		return LocalIP->ToString(false); // false = don't include port
	}
	
	return FString("127.0.0.1"); // Fallback to localhost
}