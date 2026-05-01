// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AGridGenerator.generated.h"

class ASnakePawn;
class UInstancedStaticMeshComponent;

UENUM(BlueprintType)
enum class EGridDensity : uint8 { Off, Low, Medium, High };

UENUM(BlueprintType)
enum class EFeatureLength : uint8 { Short, Medium, Long };

UENUM(BlueprintType)
enum class ECellType : uint8 { Empty, Flat, RampUp, RampDown, Elevated, Hole, Blocked };

UENUM(BlueprintType)
enum class EOccupierType: uint8 { None, Food, SnakeBody, SnakeHead, Obstacle };

USTRUCT(BlueprintType)
struct FGridCell
{
	GENERATED_BODY()

	UPROPERTY()
	ECellType CellType = ECellType::Empty;

	UPROPERTY()
	float ZOffset = 0.0f;

	UPROPERTY()
	float YawRotation = 0.0f;

	// Tiles where snake can't turn (Tunnels and Bridges)
	UPROPERTY()
	bool bIsForcedPath = false;

	// Locked trajectory if ForcedPath
	UPROPERTY()
	FVector ForcedDir = FVector::ZeroVector;

	// layer helper
	int32 Layer = 0;
};

USTRUCT(BlueprintType)
struct FGridOccupancy
{
	GENERATED_BODY()
	UPROPERTY()
	EOccupierType Type = EOccupierType::None;
	UPROPERTY()
	AActor* OccupierActor = nullptr;
};

USTRUCT()
struct FGridMoveResult
{
	GENERATED_BODY()
	UPROPERTY()
	EOccupierType HitType = EOccupierType::None;
	UPROPERTY()
	AActor* HitActor = nullptr;
};

UCLASS()
class SIDE_SCROLLER_TEST_API AAGridGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAGridGenerator();
	
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_MapSeed, Category="Grid Settings")
	int32 MapSeed = 12345;

	bool bHasGeneratedMap = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 Width = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 Height = 20;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	float TileSize = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Features")
	EGridDensity BridgeDensity = EGridDensity::Off;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Features")
	EFeatureLength BridgeLength = EFeatureLength::Short;

	// Meshes and Instanced Rendering stuff
	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* FloorISMC;

	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* WallISMC;

	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* RampISMC;

	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* TunnelISMC;

	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* BridgeISMC;
	
	UFUNCTION()
	void OnRep_MapSeed();

	TArray<FGridCell> GridData;
	TMap<FIntVector, FGridOccupancy> OccupancyMap;
	// replication function
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PreInitializeComponents() override;
	const int32 TotalLayers = 3;
	
	void RegisterSnake(ASnakePawn* Snake, const TArray<FIntVector>& InitialSegments);
	FGridMoveResult RequestMove(ASnakePawn* Snake, FIntVector NewHeadPos, FIntVector OldTailPos, bool bIsGrowing);
	bool TryClaimCell(FIntVector CellLocation, AActor* Requestor, EOccupierType OccupierType);
	void ClearCell(FIntVector CellLocation);
	
	TArray<FIntVector> GetAllEmptyCells();
	bool IsTerrainWalkable(FIntVector Location) const;
	FVector GridToWorld(FIntVector GridLocation) const;
	FIntVector WorldToGridLocation(const FVector& WorldPosition) const;

	// Helper functions for indexing and 2D to 1D conversion
	int32 GetIndex(int32 X, int32 Y, int32 Z) const { return (Z * Width * Height) + (Y * Width) + X; }
	bool IsInBounds(int32 Xpos, int32 Ypos) const { return Xpos >= 0 && Xpos < Width && Ypos >= 0 && Ypos < Height; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GenerateGrid();
	void ClearGrid();
	void WrapWalls();
	void TryPlaceBridge(FRandomStream& Stream);
	void TryPlaceHole(FRandomStream& Stream);
};
