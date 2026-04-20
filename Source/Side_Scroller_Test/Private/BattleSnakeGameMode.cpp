// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleSnakeGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "SnakeGameInstance.h"
#include "BattleSnakeGameState.h"
#include "AGridGenerator.h"
#include "SnakePawn.h"
#include "SnakePlayerState.h"
#include "SnakePlayerController.h"

ABattleSnakeGameMode::ABattleSnakeGameMode()
{
	// Point this GameMode to use your specific Battle classes
	GameStateClass = ABattleSnakeGameState::StaticClass();
	PlayerStateClass = ASnakePlayerState::StaticClass();
	PlayerControllerClass = ASnakePlayerController::StaticClass();
	
	// pawn will be handled in PostLogin
	DefaultPawnClass = nullptr;
}

FString ABattleSnakeGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	FString InName = UGameplayStatics::ParseOption(Options, TEXT("PlayerName"));

	if (NewPlayerController)
	{
		if (InName.IsEmpty() && NewPlayerController->IsLocalController())
		{
			if (USnakeGameInstance* GI = GetGameInstance<USnakeGameInstance>())
			{
				InName = GI->UserPlayerName;
			}
		}

		// Fallback
		if (InName.IsEmpty()) InName = TEXT("UnknownSnakePlayer");

		PendingNames.Add(NewPlayerController, InName);
	}
	return ErrorMessage;
}

void ABattleSnakeGameMode::PostLogin(APlayerController* NewPlayerController)
{
	Super::PostLogin(NewPlayerController);
    
	// spawn position
	AAGridGenerator* GridGen = Cast<AAGridGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AAGridGenerator::StaticClass()));
	if (!GridGen) return;
	
	// get player index
	int32 PlayerIdx = GetGameState<AGameState>()->PlayerArray.Num() - 1;
	
	// Calculate quadrant coords
	// 5 tile buffer for player spawn point
	FIntPoint SpawnGridPos;
	switch (PlayerIdx)
	{
		case 0: SpawnGridPos = FIntPoint(5, 5); break;										// bottom left
		case 1: SpawnGridPos = FIntPoint(GridGen->Width - 6, GridGen->Height - 6); break;	// top right
		case 2: SpawnGridPos = FIntPoint(5, GridGen->Height - 6); break;					// top left
		case 3: SpawnGridPos = FIntPoint(GridGen->Width - 6, 5); break;						// bottom right
		default: SpawnGridPos = FIntPoint(GridGen->Width / 2, GridGen->Height / 2); break;	// default to center 
	}
	
	// convert grid to world position
	// spawn on layer 1
	int32 ZIndex = GridGen->GetIndex(SpawnGridPos.X, SpawnGridPos.Y, 1);
	float Zposition = (GridGen->GridData.IsValidIndex(ZIndex)) ? GridGen->GridData[ZIndex].ZOffset : 0.0f;
	
	FVector WorldSpawnLocation = FVector(SpawnGridPos.X * GridGen->TileSize,
										SpawnGridPos.Y * GridGen->TileSize, Zposition);
	
	// spawn actor snake pawn
	if (SnakePawnClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		ASnakePawn* NewSnakePawn = GetWorld()->SpawnActor<ASnakePawn>(SnakePawnClass, WorldSpawnLocation,
																FRotator::ZeroRotator, SpawnParams);
		
		if (NewSnakePawn)
		{
			NewPlayerController->Possess(NewSnakePawn);
			
			UE_LOG(LogTemp, Log, TEXT("Spawned player %d in grid %s at world space %s"), PlayerIdx, *SpawnGridPos.ToString(), *WorldSpawnLocation.ToString());
		}
	}
	// if relogging in or hard transition make sure name gets populated
	if (FString* FoundName = PendingNames.Find(NewPlayerController))
	{
		ASnakePlayerState* PS = NewPlayerController->GetPlayerState<ASnakePlayerState>();
		if (PS)
		{
			PS->SnakeName = *FoundName;
			PS->OnRep_SnakeName();
		}
		PendingNames.Remove(NewPlayerController);
	}
	
	if (NewPlayerController)
	{
		// Get the PlayerState that just traveled from the Lobby
		ASnakePlayerState* PS = NewPlayerController->GetPlayerState<ASnakePlayerState>();
		
		if (PS)
		{
			// Now log the actual name the player chose in the menu
			UE_LOG(LogTemp, Log, TEXT("Snake [%s] has successfully arrived in the Battle!"), *PS->SnakeName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("A player arrived, but their PlayerState is missing!"));
		}
	}
	
}