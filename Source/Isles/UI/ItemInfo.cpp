// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ItemInfo.h"

#include "IsleInfo.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Objects/Isle/IsleBase.h"


void UItemInfo::NativeConstruct()
{
	Super::NativeConstruct();

	Add->OnClicked.AddDynamic(this, &UItemInfo::AddQuantity);
	Remove->OnClicked.AddDynamic(this, &UItemInfo::RemoveQuantity);
	BuySellButton->OnClicked.AddDynamic(this, &UItemInfo::MakeTrade);
}

void UItemInfo::SetGoodsInfo(TSharedPtr<FGoods> Info)
{
	if (Info.IsValid())
	{
		GoodsInfo = *Info;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" UItemInfo::SetGoodsInfo ItemInfo is invalid"));
	}
}


void UItemInfo::AddQuantity()
{
	Quantity++;
	if (Quantity > 0)
	{
		BuySellButton->SetVisibility(ESlateVisibility::Visible);
		BuySellText->SetText(FText::FromString(FString("Buy")));
		bBuyItem = true;
	}
	else if (Quantity == 0)
	{
		
		BuySellButton->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UItemInfo::RemoveQuantity()
{
	Quantity--;
	if (Quantity < 0)
	{
		BuySellButton->SetVisibility(ESlateVisibility::Visible);
		BuySellText->SetText(FText::FromString(FString("Sell")));
		bBuyItem = false;
	}
	else if ( Quantity == 0)
	{
		BuySellButton->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UItemInfo::MakeTrade()
{
	if (ParentPtr)
	{
		bBuyItem
			? ParentPtr->CurrentIsle->BuyItems(ParentPtr->CurrentShip, GoodsInfo.ItemName, Quantity)
			: ParentPtr->CurrentIsle->SellItems(ParentPtr->CurrentShip, GoodsInfo.ItemName, FMath::Abs(Quantity));

		ParentPtr->IslesForTrade->ClearChildren();
		ParentPtr->UpdateIsleRating();
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UItemInfo::MakeTrade get ship failed"));
	}
}

void UItemInfo::ToggleBuySellButtons()
{
	ParentPtr->CurrentShip
	? BuySellBlock->SetVisibility(ESlateVisibility::Visible)
	: BuySellBlock->SetVisibility(ESlateVisibility::Hidden);
}


