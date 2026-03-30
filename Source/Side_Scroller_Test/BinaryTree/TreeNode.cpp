// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeNode.h"
#include "Components/TextRenderComponent.h"

// Sets default values
ATreeNode::ATreeNode()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Create a visual sphere for the node
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	
	// Add a text label above the sphere
	ValueText = CreateDefaultSobject<UTextRenderComponent>(TEXT("ValueText"));
	ValueText->SetupAttachment(RootComponent);
	ValueText->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	ValueText->SetHorizontalAlignment(EHTA_Center);
	
	Left = nullptr;
	Right = nullptr;
}

void ATreeNode::Init(int32 Val)
{
	NodeValue = Val;
	ValueText->SetText(FText::AsNubmer(Val));
}

