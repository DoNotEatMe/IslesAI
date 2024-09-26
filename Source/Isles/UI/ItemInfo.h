// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IslesGameModebase.h"
#include "Blueprint/UserWidget.h"
#include "ItemInfo.generated.h"

class AShipBase;
class UIsleInfo;
class UButton;
class UTextBlock;
class USizeBox;
struct FGoods;
/**
 * 
 */
UCLASS()
class ISLES_API UItemInfo : public UUserWidget
{
	GENERATED_BODY()


	virtual void NativeConstruct() override;
	
public:

	void SetGoodsInfo(TSharedPtr<FGoods>);

	UPROPERTY(BlueprintReadOnly)
	FGoods GoodsInfo;

	UPROPERTY(meta = (BindWidget))
	USizeBox* BuySellBlock;


	UPROPERTY(BlueprintReadOnly)
	int Quantity;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Count;

	UFUNCTION()
	void AddQuantity();

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BuySellText;

	UFUNCTION()
	void RemoveQuantity();

	UPROPERTY(meta = (BindWidget))
	UButton* Add;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Remove;

	UPROPERTY()
	bool bBuyItem;

	UPROPERTY(meta = (BindWidget))
	UButton* BuySellButton;

	UFUNCTION()
	void MakeTrade();

	

	UPROPERTY()
	UIsleInfo* ParentPtr;

	UFUNCTION()
	void ToggleBuySellButtons();
	
	

	

	
};
