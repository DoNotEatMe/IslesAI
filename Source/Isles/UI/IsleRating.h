// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IsleRating.generated.h"

class UIsleInfo;
class AShipBase;
class UButton;
class AIsleBase;
/**
 * 
 */
UCLASS()
class ISLES_API UIsleRating : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadOnly)
	float Rating;

	UPROPERTY(BlueprintReadOnly)
	FString IsleName;

	UPROPERTY()
	AIsleBase* IslePtr;

	UPROPERTY()
	UIsleInfo* ParentPtr;

	UPROPERTY()
	AShipBase* ShipToMove;

	UPROPERTY(meta=(BindWidget))
	UButton* MoveToIslandButton;

	UFUNCTION()
	void MoveToIsland();
};
