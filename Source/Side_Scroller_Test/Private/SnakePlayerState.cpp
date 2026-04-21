// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePlayerState.h"
#include "Net/UnrealNetwork.h" // <--- CRITICAL INCLUDE for replication

void ASnakePlayerState::OnRep_SnakeName()
{
	// This runs on Clients when SnakeName is updated from the server
	UE_LOG(LogTemp, Log, TEXT("SnakeName replicated to client: %s"), *SnakeName);
}

void ASnakePlayerState::Server_SetSnakeName_Implementation(const FString& NewName)
{
	SnakeName = NewName;
	// On the Server, OnRep functions don't trigger automatically, 
	// so we call it manually if the server needs to do something logic-wise.
	OnRep_SnakeName(); 
}

void ASnakePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASnakePlayerState, SnakeName);
	DOREPLIFETIME(ASnakePlayerState, SpawnIndex);
}