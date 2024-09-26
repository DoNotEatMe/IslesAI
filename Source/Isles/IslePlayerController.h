// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IslePlayerController.generated.h"

class UDaysCount;
/**
 * 
 */
UCLASS()
class ISLES_API AIslePlayerController : public APlayerController
{
	GENERATED_BODY()


	AIslePlayerController();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY()
	UDaysCount* DaysCountWidget;

	UPROPERTY()
	TSubclassOf<UDaysCount> DaysCountWidgetClass;
	
};
