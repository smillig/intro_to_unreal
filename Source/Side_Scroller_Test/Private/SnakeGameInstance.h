// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SnakeGameInstance.generated.h"

UENUM(BlueprintType)
enum class EPlayMode : uint8
{
	None			UMETA(DisplayName="None"),
	Solo			UMETA(DisplayName="Solo"),
	BattleRoyale	UMETA(DisplayName="Battle Royale"),
	CoOp			UMETA(DisplayName="Co-op")
};

UCLASS()
class USnakeGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	EPlayMode SelectedMode = EPlayMode::None;
	
	UFUNCTION(BlueprintCallable)
	void HostGame(FString Port);
	
	UFUNCTION(BlueprintCallable)
	void JoinGame(FString IPAddress, FString Port);
	
	UPROPERTY(BlueprintReadWrite)
	FString UserPlayerName = "SnakePlayer";

	UPROPERTY(BlueprintReadWrite)
	FString HostPlayerName = "Host";

	UPROPERTY(BlueprintReadWrite)
	FString ClientPlayerName = "Client";

	UPROPERTY(BlueprintReadWrite)
	FString HostIPAddress = "127.0.0.1";
	
	UPROPERTY(BlueprintReadWrite)
	FString HostPort = "7755";
};
