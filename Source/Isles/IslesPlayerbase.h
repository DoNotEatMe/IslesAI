// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/Pawn.h"

#include "IslesPlayerbase.generated.h"

class UDaysCount;
class AShipTest;
class AIsleBase;
class AShipBase;
class UIsleInfo;
class UCameraComponent;
class USpringArmComponent;
class UCapsuleComponent;
class UFloatingPawnMovement;
class InputMappingContext;

UCLASS()
class ISLES_API AIslesPlayerBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AIslesPlayerBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Gold = 10000.f;

	UPROPERTY()
	AShipBase* ShipInFocus;

	UFUNCTION()
	void CallIsleInfo(AIsleBase* Isle);

	UPROPERTY()
	UIsleInfo* IsleInfo;

	UFUNCTION()
	void AddShipToFleet(AShipBase* Ship);
	
	UFUNCTION()
	void UpdateFleetNumbers();
	

private:

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
	
	UPROPERTY(VisibleAnywhere)
	UFloatingPawnMovement* MovementComponent;
	
	UPROPERTY(EditAnywhere, Category="Settings | Inputs | IMC")
	UInputMappingContext* IMC_IslePlayer;

	UPROPERTY(EditAnywhere, Category="Settings | Inputs | IA")
	UInputAction* IA_MoveForward;

	UPROPERTY(EditAnywhere, Category="Settings | Inputs | IA")
	UInputAction* IA_MoveRight;

	UPROPERTY(EditAnywhere, Category="Settings | Inputs | IA")
	UInputAction* IA_RotateCamera;
	
	UPROPERTY(EditAnywhere, Category="Settings | Inputs | IA")
	UInputAction* IA_Mouse;

	UPROPERTY(EditAnywhere, Category="Settings | Inputs | IA")
	UInputAction* IA_MouseWheel;

	UFUNCTION()
	void MoveForward(const FInputActionValue& Value);
	
	UFUNCTION()
	void MoveRight(const FInputActionValue& Value);

	UFUNCTION()
	void RotateCamera(const FInputActionValue& Value);

	UFUNCTION()
	void MouseControl(const FInputActionValue& Value);

	UFUNCTION()
	void MouseZoom(const FInputActionValue& Value);

	UPROPERTY()
	APlayerController* PawnController;

	UPROPERTY()
	FHitResult MouseHit;

	UPROPERTY()
	TSubclassOf<UIsleInfo> IsleInfoClass;

	UPROPERTY()
	TArray<AShipBase*> PlayerShips;




	
	
};
