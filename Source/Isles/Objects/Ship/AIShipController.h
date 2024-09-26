// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ShipBase.h"
#include "AIShipController.generated.h"

class UNavigationSystemV1;
class AIsleBase;
class AShipBase;
/**
 * 
 */
UCLASS()
class ISLES_API AAIShipController : public AAIController
{
	GENERATED_BODY()

	AAIShipController();

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY()
	AIslesGameModeBase* IsleGameMode;
	
	

	UFUNCTION()
	void SellTimer();

	UPROPERTY()
	FTimerHandle SellTimerHandle;

	UPROPERTY()
	float ElapsedTime = 0;
	
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	

public:

	UPROPERTY()
	AShipBase* ControlledPawn;
	
	UPROPERTY()
	UNavigationSystemV1* NavSystem;
	
	UFUNCTION(BlueprintCallable)
	void ClickToMove(AIsleBase* TargetIsle);
		
	UFUNCTION()
	bool GetProjectedNavPoints(const FVector& StartPoint, const FVector& EndPoint);

	
};
