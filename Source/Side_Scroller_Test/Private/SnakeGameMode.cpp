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
	
	// apply game mode
	if (USnakeGameInstance* GI = Cast<USnakeGameInstance>(GetGameInstance()))
	{
		if (ASnakeGameState* GS = GetGameState<ASnakeGameState>())
		{
			GS->CurrentPlayMode = GI->SelectedMode;
			GS->CurrentLevel = GI->CurrentSnakeLevel;
			UE_LOG(LogTemp, Log, TEXT("ASnakeGameMode::BeginPlay() Level: %d ,Mode: %d"), GS->CurrentLevel, GS->CurrentPlayMode);
		}
	}
	
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

// this has some unforeseen problems, I think on tick in grid should find occupied cells; food and snake
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
	TArray<FIntVector> AvailableCells = GridGen->GetAllEmptyCells();
	
	// now pick a random spawn point from valid (WalkableCells)
	if (AvailableCells.Num() > 0)
	{
		FIntVector RandomCell = AvailableCells[FMath::RandRange(0, AvailableCells.Num() - 1)];
		
		// randomly pick food type
		float Chance = FMath::FRand();
		TSubclassOf<AFoodActor> FoodToSpawn = AppleClass;
		if (Chance > 0.8f) FoodToSpawn = EggClass;
		else if (Chance < 0.1f) FoodToSpawn = MushroomClass;
		
		// finally spawn it
		FVector SpawnLocation = GridGen->GridToWorld(RandomCell) + FVector(0, 0, 80.0f);
		UE_LOG(LogTemp, Log, TEXT("Spawning food at X: %d, Y: %d, Z: %d"), RandomCell.X, RandomCell.Y, RandomCell.Z);

		AFoodActor* NewFood = GetWorld()->SpawnActor<AFoodActor>(FoodToSpawn, SpawnLocation, FRotator::ZeroRotator);
		// ptr check in case food spawns on another food
		if (NewFood)
		{
			GridGen->TryClaimCell(RandomCell, NewFood, EOccupierType::Food);
			NewFood->GridLocation = RandomCell;
			NewFood->OnFoodEaten.AddDynamic(this, &ASnakeGameMode::HandleFoodEaten);
		}
	}
}

FIntVector ASnakeGameMode::GetNextSpawnCell()
{
	// 1. Get current slot and advance the Round Robin for the NEXT person
	int32 CurrentSlot = SpawnSlot;
	SpawnSlot = (SpawnSlot + 1) % 4; 

	AAGridGenerator* LocalGridGen = Cast<AAGridGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AAGridGenerator::StaticClass()));
	
	if (LocalGridGen)
	{
		switch (CurrentSlot)
		{
		case 0: return FIntVector(5, 5, 1);                                              // Bottom Left
		case 1: return FIntVector(LocalGridGen->Width - 6, LocalGridGen->Height - 6, 1); // Top Right
		case 2: return FIntVector(5, LocalGridGen->Height - 6, 1);                       // Top Left
		case 3: return FIntVector(LocalGridGen->Width - 6, 5, 1);                        // Bottom Right
		}
	}
	
	return FIntVector(5, 5, 1); // Fallback
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
	
	// UE_LOG(LogTemp, Warning, TEXT("Game Ended"));

	// Tell all controllers the game is over to show the UI
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ASnakePlayerController* PC = Cast<ASnakePlayerController>(It->Get()))
		{
			PC->Client_ShowGameOverScreen();
		}
	}
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}