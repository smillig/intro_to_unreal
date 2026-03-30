// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeManager.h"


// Sets default values
ATreeManager::ATreeManager() { Root = nullptr; }

// Called when the game starts or when spawned
void ATreeManager::BeginPlay()
{
	Super::BeginPlay();
	
	// Simple tree example
	TArray<int32> Values = { 50, 25, 75, 10, 34, 60, 99 };
	for (int32 V : Values)
	{
		AddValue(V);
	}
}

void ATreeManager::AddValue(int32 Value)
{
	Root = RecursiveInsert(Root, Value, GetActorLocation(), 1);
}

ATreeNode* ATreeManager::RecursiveInsert(ATreeNode* CurrentNode, int32 Value, FVector Location, int32 Depth)
{
	if (CurrentNode == nullptr)
	{
		// Spawning the Actor in the World
		FActorSpawnParameters SpawnParams;
		ATreeNode* NewNode = GetWorld()->SpawnActor<ATreeNode>(ATreeNode::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);
        
		if (NewNode)
		{
			NewNode->Init(Value);
			if (NodeMesh) NewNode->StaticMesh->SetStaticMesh(NodeMesh);
		}
		return NewNode;
	}

	// Standard BST Logic + 3D Positioning
	if (Value < CurrentNode->NodeValue)
	{
		FVector LeftLoc = Location + FVector(0, -HorizontalSpacing / Depth, -VerticalSpacing);
		CurrentNode->Left = RecursiveInsert(CurrentNode->Left, Value, LeftLoc, Depth + 1);
        
		// Visual connection line
		DrawDebugLine(GetWorld(), Location, LeftLoc, FColor::Green, true, -1, 0, 5.0f);
	}
	else
	{
		FVector RightLoc = Location + FVector(0, HorizontalSpacing / Depth, -VerticalSpacing);
		CurrentNode->Right = RecursiveInsert(CurrentNode->Right, Value, RightLoc, Depth + 1);

		DrawDebugLine(GetWorld(), Location, RightLoc, FColor::Red, true, -1, 0, 5.0f);
	}

	return CurrentNode;
}
