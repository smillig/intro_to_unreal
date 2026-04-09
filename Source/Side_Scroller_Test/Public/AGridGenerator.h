// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "AGridGenerator.generated.h"

UENUM(BlueprintType)
enum class EGridDensity : uint8 { Off, Low, Medium, High };

UENUM(BlueprintType)
enum class EfeaturLength : uint8 { Short, Medium, Long };

UENUM(BlueprintType)
enum class ECellType : uint8 { Empty, Flat, RampUp, RampDown, Elevated, Hole, Blocked };

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

	// Locked tragectory if ForcedPath
	UPROPERTY()
	FVector ForcedDir = FVector::ZeroVector;

	// layer helper
	int32 Layer = 0;
};


UCLASS()
class SIDE_SCROLLER_TEST_API AAGridGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAGridGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 Width = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 Height = 20;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 MapSeed = 12345;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	float TileSize = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Features")
	EGridDensity BridgeDensity = EGridDensity::Off;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Features")
	EfeaturLength BridgeLength = EfeaturLength::Short;

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

protected:
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;
	const int32 TotalLayers = 3;
	TArray<FGridCell> GridData;

	void GenerateGrid();
	void ClearGrid();
	void TryPlaceBridge();
	void TryPlaceHole();

	// Helper functions for indexing and 2D to 1D conversion
	int32 GetIndex(int32 Xpos, int32 Ypos, int32 Zpos) const { return (Zpos * Width * Height) + (Ypos * Width) + Xpos; }
	bool IsInBounds(int32 Xpos, int32 Ypos) const { return Xpos >= 0 && Xpos < Width && Ypos >= 0 && Ypos < Height; }
	
	TArray<FIntPoint> GetWalkableCells(int32 Layer) const;
};
