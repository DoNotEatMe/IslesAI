// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadUnload.generated.h"

/**
 * 
 */
UCLASS()
class ISLES_API ULoadUnload : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	float Progress;

	UPROPERTY(BlueprintReadOnly)
	bool bBuy;
	
};
