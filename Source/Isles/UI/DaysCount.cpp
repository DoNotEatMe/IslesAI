// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DaysCount.h"

#include "IslesGameModebase.h"
#include "IslesPlayerbase.h"
#include "Kismet/GameplayStatics.h"

void UDaysCount::NativeConstruct()
{
	Super::NativeConstruct();

	// Listen to dispatcher for day change
	AIslesGameModeBase* GameMode = Cast<AIslesGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!ensure(GameMode != nullptr)) return;
	GameMode->OnDayPassed.AddDynamic(this, &UDaysCount::UpdateDay);
	GameMode->GoldChanged.AddDynamic(this,&UDaysCount::UpdateGold);
}

void UDaysCount::UpdateDay()
{
	CurrentDay++;
}

void UDaysCount::UpdateGold()
{
	AIslesPlayerBase* Player = Cast<AIslesPlayerBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!ensure(Player != nullptr)) return;

	CurrentGold = Player->Gold;
}


