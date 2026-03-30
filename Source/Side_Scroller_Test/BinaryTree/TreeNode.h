// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "TreeNode.generated.h"

UCLASS()
class SIDE_SCROLLER_TEST_API ATreeNode : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATreeNode();

	void Init(int32 Val);
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* ValueText;
	
	// DSA Pointers
	ATreeNode* Left;
	ATreeNode* Right;
	int32 NodeValue;
	
};
