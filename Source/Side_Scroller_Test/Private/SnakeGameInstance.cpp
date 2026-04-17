// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void USnakeGameInstance::HostGame(FString Port)
{
	UWorld* World = GetWorld();
	if (World)
	{
		// HostPort = Port;
		// HostPlayerName = UserPlayerName;
		FString Options = FString::Printf(TEXT("listen?port=%s?PlayerName=%s"), *Port, *UserPlayerName);
		UGameplayStatics::OpenLevel(World, FName("Lvl_Lobby"), true, Options);
	}
}

void USnakeGameInstance::JoinGame(FString IPAddress, FString Port)
{
	APlayerController* PC = GetFirstLocalPlayerController();
	if (PC)
	{
		// ClientPlayerName = UserPlayerName; // Store the client's name
		// HostIPAddress = IPAddress; // Store the IP they're connecting to
		// FString URL = FString::Printf(TEXT("%s:%s?PlayerName=%s"), *IPAddress, *Port, *UserPlayerName);
		// hacky forced garbage
		FString URL = FString::Printf(TEXT("%s:7777?PlayerName=%s"), *IPAddress, *UserPlayerName);
		PC->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
	}
}