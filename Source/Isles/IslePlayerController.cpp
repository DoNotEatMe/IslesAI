// Fill out your copyright notice in the Description page of Project Settings.


#include "IslePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "UI/DaysCount.h"

AIslePlayerController::AIslePlayerController()
{

	ConstructorHelpers::FClassFinder<UDaysCount> FindWidget(TEXT("/Game/UI/WBP_DaysCount"));
	if (FindWidget.Succeeded())
	{
		DaysCountWidgetClass = FindWidget.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AIslesPlayerBase::AIslesPlayerBase FindWidget failed"));
	}
	
	
}

void AIslePlayerController::BeginPlay()
{
	Super::BeginPlay();

	DaysCountWidget = CreateWidget<UDaysCount>(this, DaysCountWidgetClass);
	DaysCountWidget->AddToViewport();
}
