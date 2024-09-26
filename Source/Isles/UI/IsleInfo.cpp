// Fill out your copyright notice in the Description page of Project Settings.


#include "IsleInfo.h"

#include "IslesGameModeBase.h"
#include "ItemInfo.h"
#include "IsleRating.h"
#include "IslesPlayerbase.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/Isle/IsleBase.h"
#include "Objects/Ship/ShipBase.h"

UIsleInfo::UIsleInfo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UItemInfo> FindItemInfo(TEXT("/Game/UI/WBP_ItemInfo"));
	if (FindItemInfo.Succeeded())
	{
		ItemInfoLine = FindItemInfo.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UIsleInfo::NativeConstruct FindItemInfo failed"));
	}

	ConstructorHelpers::FClassFinder<UIsleRating> FindIsleRating(TEXT("/Game/UI/WBP_IsleRate"));
	if (FindIsleRating.Succeeded())
	{
		IsleRating = FindIsleRating.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UIsleInfo::NativeConstruct FindIsleRating failed"));
	}

	ConstructorHelpers::FClassFinder<AShipBase> ShipClassFinder(TEXT("/Game/Objects/Ship/BP_Ship"));
	if (ShipClassFinder.Succeeded())
	{
		ShipClass = ShipClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UIsleInfo::NativeConstruct FindIsleRating failed"));
	}
}

void UIsleInfo::NativeConstruct()
{
	Super::NativeConstruct();

	AIslesGameModeBase* GameMode = Cast<AIslesGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!ensure(GameMode != nullptr)) return;
	GameMode->OnDayPassed.AddDynamic(this, &UIsleInfo::DispatcherGoodsUpdate);
	GameMode->OnProductBuySellUpdate.AddDynamic(this, &UIsleInfo::DispatcherGoodsUpdate);

	AutoButton->OnClicked.AddDynamic(this, &UIsleInfo::RunTrade);
	AddShip->OnClicked.AddDynamic(this, &UIsleInfo::BuyShip);
}

void UIsleInfo::UpdateItemList(AIsleBase* Isle)
{
	if (Isle)
	{
		CurrentIsle = Isle;

		// not best method to find first, but in case small numbers - ok
		if (Isle->MooredShips.Num() > 0)
		{
			for (auto& Ship : Isle->MooredShips)
			{
				if (!Ship->bIsAutoPathing)
				{
					CurrentShip = Ship;
					AutoButtonVisibility();

					if (this->GetVisibility() != ESlateVisibility::Visible)
					{
						IslesForTrade->ClearChildren();
					}
					break;
				}
			}
		}
		else
		{
			AutoButtonVisibility();
			CurrentShip = nullptr;
		}
	}

	HelloText->SetText(FText::FromString(FString::Printf(TEXT("Goods on %s:"), *Isle->GetName())));

	int32 ItemCount = Isle->CurrentGoodsInfo.Num();
	int32 CurrentChildCount = GoodsGridPanel->GetChildrenCount() - 1;

	if (CurrentChildCount != ItemCount)
	{
		GoodsGridPanel->ClearChildren();

		// header
		UItemInfo* NewWidget = CreateWidget<UItemInfo>(this, ItemInfoLine);
		NewWidget->BuySellBlock->SetVisibility(ESlateVisibility::Hidden);
		GoodsGridPanel->AddChild(NewWidget);

		// main content
		int RowIndex = 1;
		for (auto& Item : Isle->CurrentGoodsInfo)
		{
			NewWidget = CreateWidget<UItemInfo>(this, ItemInfoLine);
			NewWidget->ParentPtr = this;
			NewWidget->ToggleBuySellButtons();
			UPanelSlot* PanelSlot = GoodsGridPanel->AddChild(NewWidget);
			if (UGridSlot* GridSlot = Cast<UGridSlot>(PanelSlot))
			{
				GridSlot->SetRow(RowIndex);
				RowIndex++;
			}
			else { UE_LOG(LogTemp, Error, TEXT("UIsleInfo::UpdateItemList GridSlot cast failed")); }
			NewWidget->SetGoodsInfo(Item);
		}
	}
	else
	{
		for (int32 i = 1; i < ItemCount + 1; ++i)
		{
			if (auto ItemChild = Cast<UItemInfo>(GoodsGridPanel->GetChildAt(i)))
			{
				ItemChild->SetGoodsInfo(Isle->CurrentGoodsInfo[i - 1]);
				ItemChild->ToggleBuySellButtons();
			}
		}
	}
}

void UIsleInfo::DispatcherGoodsUpdate()
{
	if (this->GetVisibility() == ESlateVisibility::Visible)
	{
		UpdateItemList(CurrentIsle);
	}
}

void UIsleInfo::RunTrade()
{
	if (CurrentShip)
	{
		CurrentShip->RunAutoTrade();
		CurrentShip = nullptr;
	}
}

void UIsleInfo::UpdateIsleRating()
{
	TArray<TPair<AIsleBase*, float>> RatingArray;
	CurrentShip->GetBestIslandWithRating(RatingArray);

	// UE_LOG(LogTemp, Warning, TEXT("Ratinf Array num : %d"), RatingArray.Num());

	if (!RatingArray.IsEmpty())
	{
		int RowIndex = 0;
		int ColumnIndex = 0;
		for (auto& Isle : RatingArray)
		{
			UIsleRating* NewWidget = CreateWidget<UIsleRating>(this, IsleRating);
			UPanelSlot* PanelSlot = IslesForTrade->AddChild(NewWidget);
			NewWidget->IsleName = Isle.Key->GetName();
			NewWidget->IslePtr = Isle.Key;
			NewWidget->ShipToMove = CurrentShip;
			NewWidget->ParentPtr = this;
			NewWidget->Rating = Isle.Value;

			if (UGridSlot* GridSlot = Cast<UGridSlot>(PanelSlot))
			{
				if (RowIndex >= 4)
				{
					ColumnIndex++;
					RowIndex = 0;
				}
				GridSlot->SetRow(RowIndex);
				GridSlot->SetColumn(ColumnIndex);
				RowIndex++;
			}

			// UE_LOG(LogTemp, Warning, TEXT("Isle: %s, Rating: %f"), *Isle.Key->GetName(), Isle.Value);
		}
	}
	else { UE_LOG(LogTemp, Error, TEXT("Rating is empty")); }
}


void UIsleInfo::BuyShip()
{
	if (AIslesPlayerBase* Player = Cast<AIslesPlayerBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
	{
		if (Player->Gold > 1000)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			AShipBase* NewShip = GetWorld()->SpawnActor<AShipBase>(
				ShipClass, CurrentIsle->MooringPoint->GetComponentLocation(),
				CurrentIsle->MooringPoint->GetComponentRotation(), Params);

			NewShip->LastPortVisited = CurrentIsle;
			NewShip->bIsMoored;
			CurrentIsle->MooreShip(NewShip);
			NewShip->AddToFleet();
			UpdateItemList(CurrentIsle);
			

			Player->Gold -= 1000;
			if (AIslesGameModeBase* GM = Cast<AIslesGameModeBase>(GetWorld()->GetAuthGameMode()))
			{
				GM->GoldChanged.Broadcast();
			}
		}
	}
}

void UIsleInfo::AutoButtonVisibility()
{
	if (CurrentIsle->MooredShips.IsEmpty())
	{
		AutoButton->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		for (auto& Ship : CurrentIsle->MooredShips)
		{
			if (!Ship->bIsAutoPathing)
			{
				CurrentShip = CurrentIsle->MooredShips[0];
				AutoButton->SetVisibility(ESlateVisibility::Visible);
				break;
			}
		}
	}
}


