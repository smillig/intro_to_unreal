// Fill out your copyright notice in the Description page of Project Settings.


#include "AGridGenerator.h"

// Sets default values
AAGridGenerator::AAGridGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FloorISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FloorISMC"));
	RampISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("RampISMC"));
	WallISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WallISMC"));
}

void AAGridGenerator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	GenerateGrid();
}

void AAGridGenerator::GenerateGrid()
{
	ClearGrid();
	GridData.Init(FGridCell(), Width * Height);

	// Feature density calcualtions done here.
	int32 FeatureCount = (Width * Height) / 100; // perhaps density could be constrained from 10 to 50%
	if (BridgeDensity == EGridDensity::Low) FeatureCount /= 2;
	else if (BridgeDensity == EGridDensity::High) FeatureCount *= 2;
	else if (BridgeDensity == EGridDensity::Off) FeatureCount = 0;

	// Randomly place Features
	for (int32 i = 0; i < (FeatureCount / 2); i++)
	{
		TryPlaceBridge();
	}
	// for (int32 i = 0; i < (FeatureCount / 2); i++)
	// {
	// 	TryPlaceHole();
	// }

	// render the grid
	for (int32 y = 0; y < Height; y++)
	{
		for (int32 x = 0; x < Width; x++)
		{
			FGridCell& Cell = GridData[GetIndex(x, y)];
			FTransform CurrentTransform;
			CurrentTransform.SetLocation(FVector(x * TileSize, y * TileSize, Cell.ZOffset));
			CurrentTransform.SetRotation(FQuat(FRotator(0.0f, Cell.YawRotation, 0.0f)));

			switch (Cell.CellType)
			{
				// Probably need to add a Bridge and move elevated
				case ECellType::Flat:
				case ECellType::Elevated:
				case ECellType::Hole:
					FloorISMC->AddInstance(CurrentTransform);
					break;
				case ECellType::RampUp:
				case ECellType::RampDown:
					RampISMC->AddInstance(CurrentTransform);
					break;
				case ECellType::Blocked:
					WallISMC->AddInstance(CurrentTransform);
					break;
			}
		}
	}
}

void AAGridGenerator::TryPlaceBridge()
{
	int32 StartX = FMath::RandRange(1, Width - 10);
	int32 StartY = FMath::RandRange(1, Height - 1);

	// Decide Lenght according to settings
	int32 MidLength = 1;
	if (BridgeLength == EfeaturLength::Short) MidLength = FMath::RandRange(1, 3);
	else if (BridgeLength == EfeaturLength::Medium) MidLength = FMath::RandRange(3, 5);
	else if (BridgeLength == EfeaturLength::Long) MidLength = FMath::RandRange(5, 10);

	// Check if path is clear
	for (int i = 0; i < MidLength + 2; i++)
	{
		if (GridData[GetIndex(StartX + i, StartY)].CellType != ECellType::Flat) return;
	}

	// Place ramp up
	GridData[GetIndex(StartX, StartY)].CellType = ECellType::RampUp;
	GridData[GetIndex(StartX, StartY)].YawRotation = 0.0f;

	// Place Mid Sections
	for (int i = 1; i <= MidLength; i++)
	{
		FGridCell& MidCell = GridData[GetIndex(StartX + i, StartY)];
		MidCell.CellType = ECellType::Elevated;
		MidCell.ZOffset = TileSize;
	}

	// Place ramp down
	GridData[GetIndex(StartX + MidLength + 1, StartY)].CellType = ECellType::RampDown;
	GridData[GetIndex(StartX + MidLength + 1, StartY)].YawRotation = 180.0f;
}

void AAGridGenerator::ClearGrid()
{
	FloorISMC->ClearInstances();
	RampISMC->ClearInstances();
	WallISMC->ClearInstances();
	GridData.Empty();
}

void AAGridGenerator::TryPlaceHole()
{
}

