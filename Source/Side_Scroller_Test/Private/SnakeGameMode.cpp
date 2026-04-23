// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameMode.h"
#include "AGridGenerator.h"
#include "SnakePawn.h"
#include "SnakePlayerState.h"
#include "FoodActor.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

void ASnakeGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Find managers
	GridGen = Cast<AAGridGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AAGridGenerator::StaticClass()));
	
	SpawnFood();
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
		GetWorld()->SpawnActor<AFoodActor>(FoodToSpawn, SpawnLocation, FRotator::ZeroRotator);
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

// example code for future multiplayer collision handler
// void ASnakeGameMode::OnSnakeHitCollision(ASnakePawn* Victim, ASnakePawn* Attacker)
// {
// 	// Award points to the Attacker (if it wasn't a self-collision)
// 	if (Attacker && Attacker != Victim) {
// 		Attacker->AddScore(100);
// 	}
//
// 	// Kill the Victim
// 	Victim->SetIsDead(true);
//
// 	// Logic for respawning or Game Over
// 	RestartPlayer(Victim->GetController());
// }