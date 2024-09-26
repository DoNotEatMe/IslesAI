// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "IslesHUDBase.generated.h"

/**
 * 
 */
UCLASS()
class ISLES_API AIslesHUDBase : public AHUD
{
	GENERATED_BODY()

	AIslesHUDBase();

protected:
	virtual void BeginPlay() override;
};
