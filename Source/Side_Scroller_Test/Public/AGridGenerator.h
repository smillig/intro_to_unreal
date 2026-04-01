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
	ECellType CellType = ECellType::Flat;

	UPROPERTY()
	float ZOffset = 0.0f;

	UPROPERTY()
	float YawRotation = 0.0f;
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

	// UPROPERTY(VisibleAnywhere)
	// UInstancedStaticMeshComponent* BridgeISMC;

protected:
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;
	TArray<FGridCell> GridData;

	void GenerateGrid();
	void ClearGrid();
	void TryPlaceBridge();
	void TryPlaceHole();

	// Helper functions for indexing and 2D to 1D conversion
	int32 GetIndex(int32 Xpos, int32 Ypos) const { return Ypos * Width + Xpos; }
	bool IsInBounds(int32 Xpos, int32 Ypos) const { return Xpos >= 0 && Xpos < Width && Ypos >= 0 && Ypos < Height; }
};
