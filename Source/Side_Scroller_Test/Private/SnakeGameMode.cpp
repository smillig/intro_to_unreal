// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameMode.h"
#include "AGridGenerator.h"
#include "SnakePawn.h"
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