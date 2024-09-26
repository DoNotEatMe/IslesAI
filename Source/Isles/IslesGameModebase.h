// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IslesGameModebase.generated.h"


class AIsleBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDayPassed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeekPassed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGoldChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnProductBuySellUpdate);


// Bad Idea to make declaration here. For ships and isles i need to declare gm.h for Fgoods. Will fix it if there will be time
USTRUCT(BlueprintType)
struct FGoods : public FTableRowBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Data")
	FName ItemName = FName("default");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	int Stock_Mim = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	int Stock_Max = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	int Stock_Current = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	float BasePrice = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	float Weight = -1;
};


UCLASS()
class ISLES_API AIslesGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AIslesGameModeBase();

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle DayCountTimerHandle;

	UFUNCTION()
	void UpdateDays();
	
	UFUNCTION()
	void ChoosePorts();
	
	
public:
	// days
	UPROPERTY()
	int DaysCount = 0;

	UPROPERTY(EditAnywhere)
	int DayInSeconds = 5;

	UPROPERTY(BlueprintAssignable)
	FOnDayPassed OnDayPassed;

	UPROPERTY(BlueprintAssignable)
	FOnWeekPassed OnWeekPassed;

	UPROPERTY(BlueprintAssignable)
	FOnGoldChanged GoldChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnProductBuySellUpdate OnProductBuySellUpdate;

	UPROPERTY()
	TArray<AActor*> Isles;

	UPROPERTY(BlueprintReadOnly)
	TArray<AIsleBase*> PortIsles;
	

	// goods Info
	UPROPERTY()
	UDataTable* Data;

	UPROPERTY(EditAnywhere)
	int IslesWithPort = 5;
	
	
	
};
