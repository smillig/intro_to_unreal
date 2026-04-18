// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void USnakeGameInstance::HostGame()
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString Options = FString::Printf(TEXT("listen?PlayerName=%s"), *UserPlayerName);
		UGameplayStatics::OpenLevel(World, FName("Lvl_Lobby"), true, Options);
	}
}

void USnakeGameInstance::JoinGame(FString IPAddress, FString Port)
{
	APlayerController* PC = GetFirstLocalPlayerController();
	if (PC)
	{
		FString URL = FString::Printf(TEXT("%s:%s?PlayerName=%s"), *IPAddress, *Port, *UserPlayerName);
		PC->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
	}
}