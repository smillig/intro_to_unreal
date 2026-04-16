// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePlayerState.h"
#include "Net/UnrealNetwork.h" // <--- CRITICAL INCLUDE for replication

void ASnakePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Tell Unreal to replicate this variable from the server to all clients
	DOREPLIFETIME(ASnakePlayerState, SnakeName);
}

// Note the _Implementation suffix!
void ASnakePlayerState::Server_SetSnakeName_Implementation(const FString& NewName)
{
	SnakeName = NewName;
	
	// Optional: Log it to verify
	UE_LOG(LogTemp, Warning, TEXT("Player name set to: %s on the Server"), *SnakeName);
}
