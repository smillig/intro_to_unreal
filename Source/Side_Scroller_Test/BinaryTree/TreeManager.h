// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TreeNode.h"
#include "TreeManager.generated.h"

UCLASS()
class SIDE_SCROLLER_TEST_API ATreeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATreeManager();
	
	// The root of the binary tree
	ATreeNode* Root;
	
	// Config for spacing in 3D
	UPROPERTY(EditAnywhere, Category="Tree Settings")
	float HorizontalSpacing = 200.0f;
	
	UPROPERTY(EditAnywhere, Category="Tree Settings")
	float VerticalSpacing = 150.0f;
	
	// The Mesh for the nodes to use (assign in Blueprint)
	void AddValue(int32 Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	// Recursive insert
	ATreeNode* RecursiveInsert(ATreeNode* CurrentNode, int32 Value, FVector Location, int32 Depth);

};
