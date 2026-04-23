// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleSnakeGameMode.h"
#include "SnakeGameInstance.h"
#include "BattleSnakeGameState.h"
#include "EngineUtils.h"
#include "SnakePlayerState.h"
#include "SnakePlayerController.h"
#include "AGridGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

ABattleSnakeGameMode::ABattleSnakeGameMode()
{
	// Point this GameMode to use your specific Battle classes
	GameStateClass = ABattleSnakeGameState::StaticClass();
	PlayerStateClass = ASnakePlayerState::StaticClass();
	PlayerControllerClass = ASnakePlayerController::StaticClass();
	
	bUseSeamlessTravel = false;
}

void ABattleSnakeGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("DefaultPawnClass = %s"),
		*GetNameSafe(DefaultPawnClass));
}

void ABattleSnakeGameMode::HandleSeamlessTravelPlayer(AController*& Contr)
{
	Super::HandleSeamlessTravelPlayer(Contr);
	
	UE_LOG(LogTemp, Warning, TEXT("Player arrived: %s in seamless travel."), *Contr->GetName());
	
	ASnakePlayerState* PS = Contr->GetPlayerState<ASnakePlayerState>();
	
	UE_LOG(LogTemp, Warning, TEXT("Player arrived: %s assigned slot: %d from seamless travel."), *PS->SnakeName, PS->PlayerSlotID);
	
	USnakeGameInstance* SnGI = Cast<USnakeGameInstance>(GetGameInstance());
	
	UE_LOG(LogTemp, Warning, TEXT("Name From GI: %s in seamless travel."), *SnGI->UserPlayerName);
	
}

AActor* ABattleSnakeGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// Capture the slot number BEFORE we increment it (GetNextSpawnCell increments it automatically)
	int32 SlotForTag = SpawnSlot; 
	FName TargetTag = FName(*FString::Printf(TEXT("Start%d"), SlotForTag));

	// Check if we ALREADY spawned an invisible PlayerStart for this slot
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		if (It->PlayerStartTag == TargetTag)
		{
			// We only need to advance the counter so the next player gets a different spot
			GetNextSpawnCell(); 
			return *It;
		}
	}

	// It doesn't exist yet! Let's get the calculated spot and spawn one dynamically.
	FIntPoint SpawnGridPos = GetNextSpawnCell();
	AAGridGenerator* LocalGridGen = Cast<AAGridGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AAGridGenerator::StaticClass()));

	if (LocalGridGen)
	{
		int32 ZIndex = LocalGridGen->GetIndex(SpawnGridPos.X, SpawnGridPos.Y, 1);
		float Zposition = (LocalGridGen->GridData.IsValidIndex(ZIndex)) ? LocalGridGen->GridData[ZIndex].ZOffset : 0.0f;
		FVector WorldSpawnLocation = FVector(SpawnGridPos.X * LocalGridGen->TileSize, SpawnGridPos.Y * LocalGridGen->TileSize, Zposition + 100.0f);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		if (APlayerStart* DynamicStart = GetWorld()->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), WorldSpawnLocation, FRotator::ZeroRotator, SpawnParams))
		{
			DynamicStart->PlayerStartTag = TargetTag;
			return DynamicStart;
		}
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

void ABattleSnakeGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	
	
	ASnakePlayerState* PS = NewPlayer->GetPlayerState<ASnakePlayerState>();
	
	if (PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player arrived: %s in Handle Starting New Player."), *PS->SnakeName );
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player State not available yet in Handle Starting New Player.") );
	}
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

void ABattleSnakeGameMode::PostLogin(APlayerController* NewPlayerController)
{
	if (NewPlayerController)
	{
		ASnakePlayerState* PS = NewPlayerController->GetPlayerState<ASnakePlayerState>();
		if (PS)
		{
			// Apply Name
			if (FString* FoundName = PendingNames.Find(NewPlayerController))
			{
				PS->SnakeName = *FoundName;
				PS->OnRep_SnakeName(); // Update Host immediately
				PendingNames.Remove(NewPlayerController);
			}

			// Apply Slot ID
			if (int32* FoundSlot = PendingSlots.Find(NewPlayerController))
			{
				PS->PlayerSlotID = *FoundSlot;
				PendingSlots.Remove(NewPlayerController);
			}
		}
	}
	Super::PostLogin(NewPlayerController);
	// FORCE deterministic spawn (ONLY HERE)
	RestartPlayer(NewPlayerController);
}

FString ABattleSnakeGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	UE_LOG(LogTemp, Warning, TEXT("New Player Init: %s in Init New Player"), *NewPlayerController->GetName());
	
	ASnakePlayerState* PS = NewPlayerController->GetPlayerState<ASnakePlayerState>();
	
	if (PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player State: %s in Init New Player"), *PS->SnakeName);
	}
	
	if (NewPlayerController)
	{
		// 1. Grab the name from the URL
		FString InName = UGameplayStatics::ParseOption(Options, TEXT("PlayerName"));
		
		// Fallback if URL is empty (Usually true for the Host)
		if (InName.IsEmpty() && NewPlayerController->IsLocalController())
		{
			if (USnakeGameInstance* GI = GetGameInstance<USnakeGameInstance>())
			{
				InName = GI->UserPlayerName;
			}
		}
		if (InName.IsEmpty()) InName = TEXT("UnknownSnakePlayer");

		// 2. Store the Name for PostLogin
		PendingNames.Add(NewPlayerController, InName);

		// 3. Assign their Slot ID right now (0, 1, 2...)
		PendingSlots.Add(NewPlayerController, NextSlotID++);
	}

	return ErrorMessage;
}
