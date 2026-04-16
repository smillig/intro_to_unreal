// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void USnakeGameInstance::HostGame(FString Port)
{
	UWorld* World = GetWorld();
	if (World)
	{
		// "Listen" makes this player the server. 
		// Lobby level
		FString Options = FString::Printf(TEXT("?listen?port=%s"), *Port);
		UGameplayStatics::OpenLevel(World, FName("Lvl_Lobby"), true, Options);
	}
}

void USnakeGameInstance::JoinGame(FString IPAddress, FString Port)
{
	APlayerController* PC = GetFirstLocalPlayerController();
	if (PC)
	{
		FString URL = FString::Printf(TEXT("%s:%s"), *IPAddress, *Port);
		PC->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
	}
}