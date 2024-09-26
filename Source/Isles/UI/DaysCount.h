// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DaysCount.generated.h"

/**
 * 
 */
UCLASS()
class ISLES_API UDaysCount : public UUserWidget
{
	GENERATED_BODY()

virtual void NativeConstruct() override;

	
public:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	int CurrentDay;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	float CurrentGold;

	UPROPERTY(BlueprintReadOnly)
	int FreeShips;

	UPROPERTY(BlueprintReadOnly)
	int BotShips;

private:
	UFUNCTION()
	void UpdateDay();

	UFUNCTION()
	void UpdateGold();
};
