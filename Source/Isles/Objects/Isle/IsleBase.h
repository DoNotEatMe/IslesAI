// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Misc/Optional.h"
#include "IsleBase.generated.h"


class AIslesPlayerBase;
class AShipBase;
struct FGoods;
class AIslesGameModeBase;

UCLASS()
class ISLES_API AIsleBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AIsleBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetPort();

	UPROPERTY(BlueprintReadOnly)
	bool bIsPort = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* PortMaterial;

	UFUNCTION(BlueprintCallable)
	void SellItems(AShipBase* Ship, FName ItemName, int Quantity);

	UFUNCTION(BlueprintCallable)
	void BuyItems(AShipBase* Ship,FName ItemName, int Quantity);

	TArray<TSharedPtr<FGoods>> CurrentGoodsInfo;

	UPROPERTY(BlueprintReadOnly)
	TArray<AShipBase*> MooredShips;

	UFUNCTION()
	void MooreShip(AShipBase* Ship);

	UFUNCTION()
	void UnMoorShip(AShipBase* Ship);

	TSharedPtr<FGoods> FindItem(FName ItemName);

	UPROPERTY(EditAnywhere)
	USceneComponent* MooringPoint;

	

private:

	
	
	UFUNCTION()
	void UpdateDay();

	UFUNCTION()
	void UpdateGoods();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* IsleMesh;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PortMesh;

	UPROPERTY()
	AIslesGameModeBase* GameModePtr;

	UFUNCTION()
	void SetNewPrices();

	UFUNCTION()
	void UpdateStock();

	int WeekCount = 0;
	
};
