// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IsleRating.h"

#include "IsleInfo.h"
#include "Components/Button.h"
#include "Objects/Ship/AIShipController.h"
#include "Objects/Ship/ShipBase.h"

void UIsleRating::NativeConstruct()
{
	Super::NativeConstruct();

	MoveToIslandButton->OnClicked.AddDynamic(this, &UIsleRating::MoveToIsland);
}

void UIsleRating::MoveToIsland()
{
	ShipToMove->AIController->ClickToMove(IslePtr);
	ParentPtr->SetVisibility(ESlateVisibility::Hidden);
}
