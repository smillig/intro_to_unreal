// Fill out your copyright notice in the Description page of Project Settings.


#include "AGridGenerator.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Math/TransformCalculus3D.h"
#include "Net/UnrealNetwork.h"
#include "Util/GridIndexing3.h"

// Sets default values
AAGridGenerator::AAGridGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// networking replication setup
	bReplicates = true;
	bAlwaysRelevant = true;

	FloorISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FloorISMC"));
	RampISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("RampISMC"));
	WallISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WallISMC"));
	TunnelISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("TunnelISMC"));
	BridgeISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BridgeISMC"));

	TArray<UInstancedStaticMeshComponent*> AllComponents = { FloorISMC, RampISMC, WallISMC, TunnelISMC, BridgeISMC };
	
    for (UInstancedStaticMeshComponent* Comp : AllComponents)
    {
        if (Comp)
        {
            Comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            Comp->SetCollisionObjectType(ECC_WorldStatic);
            Comp->SetCollisionResponseToAllChannels(ECR_Block);
            
            // Set the RootComponent if it's the first one
            if (Comp == FloorISMC) { SetRootComponent(Comp); }
            else { Comp->SetupAttachment(RootComponent); }
        }
    }
}

void AAGridGenerator::OnConstruction(const FTransform& Transform)
{
	// so there's still a visual in the editor
	if (GetWorld() && !GetWorld()->IsGameWorld())
	{
		GenerateGrid();
	}
}

void AAGridGenerator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAGridGenerator, MapSeed); // Replicate the seed
}

void AAGridGenerator::OnRep_MapSeed()
{
	// Client receives the seed and generates the exact same visual grid
	GenerateGrid();
	bHasGeneratedMap = true;
}

void AAGridGenerator::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		GenerateGrid();
		bHasGeneratedMap = true;
	}
}

void AAGridGenerator::GenerateGrid()
{
	// prevent double generation
	if (GetWorld()->IsGameWorld() && bHasGeneratedMap) return;
	
	// use random seed for testing and replayability
	FRandomStream GridStream(MapSeed);
	ClearGrid();
	// Multiplied by 3 for lower, mid and upper play areas.
	GridData.Init(FGridCell(), Width * Height * TotalLayers);

	// Feature density calculations done here.
	int32 FeatureCount = (Width * Height) / 100; // perhaps density could be constrained from 10 to 50%
	if (BridgeDensity == EGridDensity::Low) FeatureCount /= 2;
	else if (BridgeDensity == EGridDensity::High) FeatureCount *= 2;
	else if (BridgeDensity == EGridDensity::Off) FeatureCount = 0;
	
	// Populate base grid
	for (int32 y = 0; y < Height; y++)
	{
		for (int32 x = 0; x < Width; x++)
		{
			GridData[GetIndex(x, y, 0)].CellType = ECellType::Flat;
		}
	}
	// Randomly place Features
	for (int32 i = 0; i < (FeatureCount / 2); i++)
	{
		TryPlaceBridge(GridStream);
	}
	// for (int32 i = 0; i < (FeatureCount / 2); i++)
	// {
	// 	TryPlaceHole(GridStream);
	// }

	// render the grid
	for (int32 z = 0; z < TotalLayers; z++)
	{
		for (int32 y = 0; y < Height; y++)
		{
			for (int32 x = 0; x < Width; x++)
			{
				FGridCell& Cell = GridData[GetIndex(x, y, z)];
				FTransform CurrentTransform;
				CurrentTransform.SetLocation(FVector(x * TileSize, y * TileSize, Cell.ZOffset));
				CurrentTransform.SetRotation(FQuat(FRotator(0.0f, Cell.YawRotation, 0.0f)));
				
				
				switch (Cell.CellType)
				{
					// Probably need to add a Bridge and move elevated
					case ECellType::Flat:
						FloorISMC->AddInstance(CurrentTransform);
                    	break;
					case ECellType::Elevated:
						BridgeISMC->AddInstance(CurrentTransform);
						break;
					case ECellType::Hole:
						TunnelISMC->AddInstance(CurrentTransform);
						break;
					case ECellType::RampUp:
					case ECellType::RampDown:
						RampISMC->AddInstance(CurrentTransform);
						break;
					case ECellType::Blocked:
						WallISMC->AddInstance(CurrentTransform);
						break;
					case ECellType::Empty:
						{}
				}
			}
		}
	}
	WrapWalls();
}

void AAGridGenerator::TryPlaceBridge(FRandomStream& Stream)
{
	int32 StartX = Stream.RandRange(1, Width - 10);
	int32 StartY = Stream.RandRange(1, Height - 1);

	// Decide Lenght according to settings
	int32 MidLength = 1;
	if (BridgeLength == EfeaturLength::Short) MidLength = Stream.RandRange(1, 3);
	else if (BridgeLength == EfeaturLength::Medium) MidLength = Stream.RandRange(3, 5);
	else if (BridgeLength == EfeaturLength::Long) MidLength = Stream.RandRange(5, 10);

	// Check if path is clear
	for (int i = 0; i < MidLength + 2; i++)
	{
		if (GridData[GetIndex(StartX + i, StartY, 1)].CellType != ECellType::Flat) return;
	}

	// Place ramp up
	GridData[GetIndex(StartX, StartY, 1)].CellType = ECellType::RampUp;
	GridData[GetIndex(StartX, StartY, 1)].YawRotation = 90.0f;
	GridData[GetIndex(StartX, StartY, 1)].ZOffset = TileSize;

	// Place Mid Sections
	for (int i = 1; i <= MidLength; i++)
	{
		FGridCell& MidCell = GridData[GetIndex(StartX + i, StartY, 2)];
		MidCell.CellType = ECellType::Elevated;
		MidCell.ZOffset = TileSize;
	}

	// Place ramp down
	GridData[GetIndex(StartX + MidLength + 1, StartY, 1)].CellType = ECellType::RampDown;
	GridData[GetIndex(StartX + MidLength + 1, StartY, 1)].YawRotation = -90.0f;
	GridData[GetIndex(StartX + MidLength + 1, StartY, 1)].ZOffset = TileSize;
}

void AAGridGenerator::TryPlaceHole(FRandomStream& Stream)
{
	
}

void AAGridGenerator::ClearGrid()
{
	FloorISMC->ClearInstances();
	RampISMC->ClearInstances();
	WallISMC->ClearInstances();
	BridgeISMC->ClearInstances();
	TunnelISMC->ClearInstances();
	GridData.Empty();
}

TArray<FIntPoint> AAGridGenerator::GetWalkableCells(int32 Layer) const
{
	TArray<FIntPoint> WalkableCells;
	for (int32 y = 0; y < Height; y++)
	{
		for (int32 x = 0; x < Width; x++)
		{
			int32 CurIndex = GetIndex(x, y, Layer);
			if (GridData.IsValidIndex(CurIndex))
			{
				if (GridData[CurIndex].CellType == ECellType::Flat || GridData[CurIndex].CellType == ECellType::Elevated)
				{
					WalkableCells.Add(FIntPoint(x, y));
				}
			}
		}
	}
	return WalkableCells;
}

void AAGridGenerator::WrapWalls()
{
	for (int32 z = 0; z < TotalLayers - 1; z++)
	{
		for (int32 y = 0; y < Height; y++)
		{
			for (int32 x = 0; x < Width; x++)
			{
				if (y == 0)
				{
					FTransform CurrentTransform;
					CurrentTransform.SetLocation(FVector(x * TileSize, -1 * TileSize, z * TileSize));
					WallISMC->AddInstance(CurrentTransform);
					if (x == 0)
					{
						FTransform BottomLCurrentTransform;
						BottomLCurrentTransform.SetLocation(FVector(-1 * TileSize, -1 * TileSize, z * TileSize));
						WallISMC->AddInstance(BottomLCurrentTransform);
					}
					if (x == Width - 1)
					{
						FTransform TopLCurrentTransform;
						TopLCurrentTransform.SetLocation(FVector((x + 1) * TileSize, -1 * TileSize, z * TileSize));
						WallISMC->AddInstance(TopLCurrentTransform);
					}
				}
				if (y == Height - 1)
				{
					FTransform CurrentTransform;
					CurrentTransform.SetLocation(FVector(x * TileSize, (y + 1) * TileSize, z * TileSize));
					WallISMC->AddInstance(CurrentTransform);
					if (x == 0)
					{
						FTransform BottomRCurrentTransform;
						BottomRCurrentTransform.SetLocation(FVector(-1 * TileSize, (y + 1) * TileSize, z * TileSize));
						WallISMC->AddInstance(BottomRCurrentTransform);
					}
					if (x == Width - 1)
					{
						FTransform TopRCurrentTransform;
						TopRCurrentTransform.SetLocation(FVector((x + 1) * TileSize, (y + 1) * TileSize, z * TileSize));
						WallISMC->AddInstance(TopRCurrentTransform);
					}
				
				}
				if (x == 0)
				{
					FTransform CurrentTransform;
					CurrentTransform.SetLocation(FVector(-1 * TileSize, y * TileSize, z * TileSize));
					if (z > 0)
					{
						// polish - Set transparency on these
						CurrentTransform.SetScale3D(FVector(1.0f, 1.0f, 0.5f));
						CurrentTransform.SetLocation(FVector(-1 * TileSize, y * TileSize, ((z * (TileSize )) - (TileSize / 4))));
					}
					WallISMC->AddInstance(CurrentTransform);
				}
				if (x == Width - 1)
				{
					FTransform CurrentTransform;
					CurrentTransform.SetLocation(FVector((x + 1) * TileSize, y * TileSize, z * TileSize));
					WallISMC->AddInstance(CurrentTransform);
				}
			}
		}
	}
}
