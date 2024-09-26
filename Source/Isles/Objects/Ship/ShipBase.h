// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ShipBase.generated.h"

class ULoadUnload;
class UWidgetComponent;
class AIslesPlayerBase;
class UBoxComponent;
class AAIShipController;
class AIslesGameModeBase;
struct FPathFollowingResult;
struct FAIRequestID;
class AIsleBase;
class UFloatingPawnMovement;
struct FPathFollowingRequestResult;
struct FGoods;

UENUM()
enum EAutoBuyResponse : uint8
{
	Success UMETA(DisplayName ="Success"),
	Failed UMETA(DisplayName ="Failed"),
	BadProductRating UMETA(DisplayName ="Success")
};

UCLASS()
class ISLES_API AShipBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AShipBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite)
	float Health;

	UPROPERTY(EditAnywhere)
	float Health_Max = 100;

	UPROPERTY(BlueprintReadWrite)
	float Weight;

	UPROPERTY(BlueprintReadWrite)
	float Weight_Max = 100;
	
	TArray<TSharedPtr<FGoods>> CurrentGoodsInfo;

	UFUNCTION()
	bool AddItems(FName ItemName, int Quantity, float Price, float ItemWeight);

	UFUNCTION()
	bool RemoveItems(FName ItemName, int Quantity, float ItemWeight);
	
	TSharedPtr<FGoods> FindItem(FName ItemName);

	UFUNCTION(BlueprintCallable)
	int GetQuantity(FName ItemName);

	UPROPERTY()
	bool bIsMoored = false;

	UPROPERTY()
	AIsleBase* LastPortVisited;

	UPROPERTY()
	AIsleBase* NextPortToVisit;

	UPROPERTY(BlueprintReadWrite)
	bool bIsAutoPathing = false;

	UFUNCTION(BlueprintCallable)
	void RunAutoTrade();

	UFUNCTION()
	EAutoBuyResponse AutoBuy();
	
	UPROPERTY()
	float ElapsedTime = 0;
	
	UFUNCTION()
	void AutoSell();
	
	UFUNCTION()
	void GetCurrentStockInfo();

	UFUNCTION()
	AIsleBase* GetBestIslandWithRating();
	void GetBestIslandWithRating(TArray<TPair<AIsleBase*, float>>& SortedIsles);

	UPROPERTY()
	AAIShipController* AIController;

	UFUNCTION()
	void AddToFleet();

	UPROPERTY()
	AIslesPlayerBase* Player;

	UPROPERTY()
	ULoadUnload* ProgressWidget;



private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ShipMesh;

	UPROPERTY(EditAnywhere)
	UFloatingPawnMovement* MovementComponent;

	UPROPERTY()
	AIslesGameModeBase* IsleGameMode;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* ProgressBar;

	UPROPERTY()
	bool bBought = false;
	

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxCollision;
	
	UFUNCTION()
	void BuyTimer();
		
	UPROPERTY()
	FTimerHandle BuySellTimerHandle;

	UFUNCTION()
	void WaitingForNewDay();

	UPROPERTY()
	FTimerHandle WaitingNewDayHandle;
	
	UPROPERTY()
	bool bStopWait = false;

	UFUNCTION()
	void DisableWaiting();
	
	UFUNCTION()
	FName BestTrade(TMap<FName, float> ProductsRating);

	UFUNCTION()
	AIsleBase* BestIsland(TMap<AIsleBase*, float>& IslesRating);

	

	UFUNCTION()
	float PathsRating(AIsleBase* TargetIsle);

	UFUNCTION()
	bool GetIslesRating(TMap<AIsleBase*, float>& IslesRating);
	
	UPROPERTY()
	bool bWeekChanged = false;
	
	

	




	
};
