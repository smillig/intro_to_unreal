// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeGameMode.h"
#include "AGridGenerator.h"
#include "SnakePawn.h"
#include "FoodActor.h"
#include "Kismet/GameplayStatics.h"

void ASnakeGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Find managers
	GridGen = Cast<AAGridGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AAGridGenerator::StaticClass()));
	SnakePawn = Cast<ASnakePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	
	SpawnFood();
}

void ASnakeGameMode::SpawnFood()
{
	if (!GridGen || !SnakePawn) return;
	
	// get cells that food can spawn on
	TArray<FIntPoint> WalkableCells = GridGen->GetWalkableCells(0);
	
	// remove points that snake occupies
	TArray<FIntPoint> SnakeHead = SnakePawn->GetOccupiedCells();
	TSet<FIntPoint> SnakeSet(SnakeHead);
	// remove SnakeHead points from WalkableCells
	int32 WriteIndex = 0;
	for (int32 ReadIndex = 0; ReadIndex < WalkableCells.Num(); ReadIndex++)
	{
		const FIntPoint& Cell = WalkableCells[ReadIndex];
		if (!SnakeSet.Contains(Cell))
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