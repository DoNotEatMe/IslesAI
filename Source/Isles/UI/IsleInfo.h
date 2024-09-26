// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IslesGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "IsleInfo.generated.h"



class UItemInfo;
class UTextBlock;
class UGridPanel;
class UIsleRating;
class UButton;
class AShipBase;
class AIsleBase;
/**
 * 
 */
UCLASS()
class ISLES_API UIsleInfo : public UUserWidget
{
	GENERATED_BODY()

	UIsleInfo(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeConstruct() override;



public:

	UPROPERTY(BlueprintReadOnly)
	AIsleBase* CurrentIsle;
	
	UPROPERTY(BlueprintReadOnly)
	AShipBase* CurrentShip;

	UPROPERTY(meta = (BindWidget))
	UButton* AutoButton;
	
	UPROPERTY(BlueprintReadWrite,meta = (BindWidget)) 
	UGridPanel* GoodsGridPanel;
	
	TArray<TSharedPtr<FGoods>> CurrentGoodsInfo;

	UFUNCTION()
	void UpdateItemList(AIsleBase* Isle);

	UPROPERTY()
	TSubclassOf<UItemInfo>  ItemInfoLine;

	UPROPERTY()
	TSubclassOf<UIsleRating>  IsleRating;

	UPROPERTY(BlueprintReadWrite,meta = (BindWidget))
	UTextBlock* HelloText;

	UFUNCTION()
	void DispatcherGoodsUpdate();

	UFUNCTION()
	void RunTrade();

	UPROPERTY(meta = (BindWidget))
	UGridPanel* IslesForTrade;

	UFUNCTION()
	void UpdateIsleRating();

	UPROPERTY(meta = (BindWidget))
	UButton* AddShip;

	UFUNCTION()
	void BuyShip();

	UPROPERTY()
	TSubclassOf<AShipBase> ShipClass;

	UFUNCTION()
	void AutoButtonVisibility();
	




};
