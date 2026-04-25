// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameMode.h"
#include "AGridGenerator.h"
#include "SnakePawn.h"
#include "SnakeGameState.h"
#include "SnakePlayerController.h"
#include "SnakePlayerState.h"
#include "FoodActor.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

void ASnakeGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Find managers
	GridGen = Cast<AAGridGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AAGridGenerator::StaticClass()));
	
	// start spawner timer loop at every two seconds
	GetWorldTimerManager().SetTimer(FoodSpawnTimerHandle, this, &ASnakeGameMode::MaintainFoodCount, 2.0f, true);
	
	// start the game timer (1 second tick rate)
	GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &ASnakeGameMode::OnMatchTimerTick, 1.0f, true);
	
	if (ASnakeGameState* GS = GetGameState<ASnakeGameState>())
	{
		GS->MatchTimeRemaining = MatchDurationSeconds;
	}
}

void ASnakeGameMode::HandleSeamlessTravelPlayer(AController*& Contr)
{
	Super::HandleSeamlessTravelPlayer(Contr);
	
	ASnakePlayerController* PC = Cast<ASnakePlayerController>(Contr);
	if (PC)
	{
		if (UPlayerHUDWidgetClass) // Check if you set it in the editor!
		{
			PC->Client_ShowPlayerHud(UPlayerHUDWidgetClass);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HUD CLASS IS NULL! Please open BP_BattleSnakeGameMode and set UPlayerHUDWidgetClass!"));
		}
	}
}
	
void ASnakeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	ASnakePlayerController* PC = Cast<ASnakePlayerController>(NewPlayer);
	if (PC)
	{
		if (UPlayerHUDWidgetClass) // Check if you set it in the editor!
		{
			PC->Client_ShowPlayerHud(UPlayerHUDWidgetClass);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HUD CLASS IS NULL! Please open BP_<This>GameMode and set UPlayerHUDWidgetClass!"));
		}
	}
}


void ASnakeGameMode::MaintainFoodCount()
{
	// get number of food on the board
	int32 CurrentFood = 0;
	for (TActorIterator<AFoodActor> It(GetWorld()); It; ++It) CurrentFood++;
	
	// spawn food till it's enough
	while (CurrentFood < MaxFoodOnBoard)
	{
		SpawnFood();
		CurrentFood++;
	}
}

void ASnakeGameMode::SpawnFood()
{
	if (!GridGen) return;
	
	// get cells that food can spawn on
	TArray<FIntPoint> WalkableCells = GridGen->GetWalkableCells(0);
	
	TSet<FIntPoint> OccupiedCellsSet;
	
	for (TActorIterator<ASnakePawn> It(GetWorld()); It; ++It)
	{
		ASnakePawn* Pawn = *It;
		if (Pawn)
		{
			OccupiedCellsSet.Append(Pawn->GetOccupiedCells());
		}
	}
	
	// remove occupied points from WalkableCells
	int32 WriteIndex = 0;
	for (int32 ReadIndex = 0; ReadIndex < WalkableCells.Num(); ReadIndex++)
	{
		const FIntPoint& Cell = WalkableCells[ReadIndex];
		if (!OccupiedCellsSet.Contains(Cell))
		{
			WalkableCells[WriteIndex++] = Cell;
		}
	}
	WalkableCells.SetNum(WriteIndex);
	
	// now pick a random spawn point from valid (WalkableCells)
	if (WalkableCells.Num() > 0)
	{
		FIntPoint RandomCell = WalkableCells[FMath::RandRange(0, WalkableCells.Num() - 1)];
		
		// randomly pick food type
		float Chance = FMath::FRand();
		TSubclassOf<AFoodActor> FoodToSpawn = AppleClass;
		if (Chance > 0.8f) FoodToSpawn = EggClass;
		else if (Chance < 0.1f) FoodToSpawn = MushroomClass;
		
		// finally spawn it
		int32 Idx = GridGen->GetIndex(RandomCell.X, RandomCell.Y, 0);
		float FloorZ = GridGen->GridData[Idx].ZOffset;
		FVector SpawnLocation = FVector(RandomCell.X * GridGen->TileSize, RandomCell.Y * GridGen->TileSize, FloorZ + 80.0f);
		UE_LOG(LogTemp, Log, TEXT("Spawning food at %d"), RandomCell.X);
		// GetWorld()->SpawnActor<AFoodActor>(FoodToSpawn, SpawnLocation, FRotator::ZeroRotator);
		AFoodActor* NewFood = GetWorld()->SpawnActor<AFoodActor>(FoodToSpawn, SpawnLocation, FRotator::ZeroRotator);
		NewFood->OnFoodEaten.AddDynamic(this, &ASnakeGameMode::HandleFoodEaten);
	}
}

FIntPoint ASnakeGameMode::GetNextSpawnCell()
{
	// 1. Get current slot and advance the Round Robin for the NEXT person
	int32 CurrentSlot = SpawnSlot;
	SpawnSlot = (SpawnSlot + 1) % 4; 

	AAGridGenerator* LocalGridGen = Cast<AAGridGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AAGridGenerator::StaticClass()));
	
	if (LocalGridGen)
	{
		switch (CurrentSlot)
		{
		case 0: return FIntPoint(5, 5);                                              // Bottom Left
		case 1: return FIntPoint(LocalGridGen->Width - 6, LocalGridGen->Height - 6); // Top Right
		case 2: return FIntPoint(5, LocalGridGen->Height - 6);                       // Top Left
		case 3: return FIntPoint(LocalGridGen->Width - 6, 5);                        // Bottom Right
		}
	}
	
	return FIntPoint(5, 5); // Fallback
}

bool ASnakeGameMode::IsCellSafe(ASnakePawn* MovingSnake, FIntPoint TargetCell)
{
	if (!MovingSnake) return false;

	ASnakePlayerState* MovingPS = MovingSnake->GetPlayerState<ASnakePlayerState>();

	// Iterate over ALL snakes in the map
	for (TActorIterator<ASnakePawn> It(GetWorld()); It; ++It)
	{
		ASnakePawn* OtherSnake = *It;
		
		// Skip if the snake is already dead
		if (OtherSnake->IsDead()) continue; 

		TArray<FIntPoint> OccupiedCells = OtherSnake->GetOccupiedCells();
		
		// Check if the cell we want to step on is currently occupied by this snake
		if (OccupiedCells.Contains(TargetCell))
		{
			ASnakePlayerState* OtherPS = OtherSnake->GetPlayerState<ASnakePlayerState>();

			if (OtherSnake == MovingSnake)
			{
				// --- SELF COLLISION ---
				if (MovingPS) MovingPS->AddScore(-50);
				MovingSnake->HandleSnakeDeath();
				return false;
			}
			else
			{
				// other snake was hit
				
				// If the cell we hit is index 0 of their occupied array, it's their Head!
				bool bHeadToHead = (TargetCell == OccupiedCells[0]);

				if (bHeadToHead)
				{
					// Head to Head: Both penalized, both die
					if (MovingPS) MovingPS->AddScore(-50);
					if (OtherPS) OtherPS->AddScore(-50);
					
					MovingSnake->HandleSnakeDeath();
					OtherSnake->HandleSnakeDeath();
				}
				else
				{
					// Hit their body. 
					// MovingSnake crashed (Victim), OtherSnake cut them off (Attacker).
					if (MovingPS) MovingPS->AddScore(-50); // Victim penalty
					if (OtherPS) OtherPS->AddScore(100);   // Attacker reward

					MovingSnake->HandleSnakeDeath();
				}
				return false;
			}
		}
	}
	// Cell is totally empty of snakes!
	return true;
}

void ASnakeGameMode::ToggleGamePause(AController* Requester)
{
	// Check current pause state and toggle it
	bool bIsCurrentlyPaused = UGameplayStatics::IsGamePaused(GetWorld());
	bool bNewPauseState = !bIsCurrentlyPaused;
	
	// pause the game
	UGameplayStatics::SetGamePaused(GetWorld(), bNewPauseState);
	
	// loop through all players and show pause menu
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ASnakePlayerController* PlayerController = Cast<ASnakePlayerController>(It->Get()))
		{
			PlayerController->Client_TogglePauseMenu(bNewPauseState);
		}
	}
}

void ASnakeGameMode::HandleFoodEaten(AFoodActor* EatenFood, ASnakePawn* Eater)
{
	if (!Eater || !EatenFood) return;

	// Give points
	if (ASnakePlayerState* PS = Eater->GetPlayerState<ASnakePlayerState>())
	{
		PS->AddScore(EatenFood->ScoreValue);
	}

	// Grow/Shrink Snake
	Eater->ModifySegments(EatenFood->GrowthAmount);
}

void ASnakeGameMode::OnMatchTimerTick()
{
	if (ASnakeGameState* GS = GetGameState<ASnakeGameState>())
	{
		GS->MatchTimeRemaining--;
		
		// Optional: Force a replication update instantly for accurate UI
		GS->ForceNetUpdate();
		
		// since host is a player we need to broadcast to them too
		GS->OnTimerUpdated.Broadcast(GS->MatchTimeRemaining);

		if (GS->MatchTimeRemaining <= 0)
		{
			EndGame();
		}
	}
}

void ASnakeGameMode::EndGame()
{
	GetWorldTimerManager().ClearTimer(MatchTimerHandle);
	GetWorldTimerManager().ClearTimer(FoodSpawnTimerHandle);

	// Tell all controllers the game is over to show the UI
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ASnakePlayerController* PC = Cast<ASnakePlayerController>(It->Get()))
		{
			PC->Client_ShowGameOverScreen();
		}
	}
}