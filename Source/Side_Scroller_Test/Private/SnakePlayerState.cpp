// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePlayerState.h"
#include "Net/UnrealNetwork.h"

void ASnakePlayerState::OnRep_SnakeName()
{
	// This runs on Clients when SnakeName is updated from the server
	UE_LOG(LogTemp, Log, TEXT("SnakeName replicated to client: %s"), *SnakeName);
}

void ASnakePlayerState::OnRep_SnakeScore()
{
	OnScoreChanged.Broadcast(SnakeScore);
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
	DOREPLIFETIME(ASnakePlayerState, PlayerSlotID);
	DOREPLIFETIME(ASnakePlayerState, SnakeScore);
}

// potential fix for Player State being lost between transitions:
void ASnakePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	// This is the magic function for Seamless Travel. 
	// It copies your custom variables from the old map to the new map!
	if (ASnakePlayerState* NewState = Cast<ASnakePlayerState>(PlayerState))
	{
		NewState->SnakeName = this->SnakeName;
		NewState->PlayerSlotID = this->PlayerSlotID;
	}
}

void ASnakePlayerState::AddScore(int32 Amount)
{
	if (HasAuthority())
	{
		SnakeScore += Amount;
		UE_LOG(LogTemp, Log, TEXT("Amount = %d added for Player: %s"), Amount, *SnakeName);
		OnScoreChanged.Broadcast(SnakeScore);
	}
}
