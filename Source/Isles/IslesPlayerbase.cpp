// Fill out your copyright notice in the Description page of Project Settings.


#include "IslesPlayerbase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "IslePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Objects/Isle/IsleBase.h"
#include "Objects/Ship/ShipBase.h"
#include "UI/DaysCount.h"
#include "UI/IsleInfo.h"


// Sets default values
AIslesPlayerBase::AIslesPlayerBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = Capsule;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Capsule);
	SpringArm->TargetArmLength = 2400.f;
	SpringArm->SetRelativeRotation(FRotator(-50.f, 0.f, 0.f));
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));

	ConstructorHelpers::FClassFinder<UIsleInfo> FindWidget(TEXT("/Game/UI/WBP_IsleInfo"));
	if (FindWidget.Succeeded())
	{
		IsleInfoClass = FindWidget.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AIslesPlayerBase::AIslesPlayerBase FindWidget failed"));
	}
	
}

void AIslesPlayerBase::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	// storing controller ref in case mouse hit tracing
	PawnController = World->GetFirstPlayerController();
	if (!ensure(PawnController != nullptr)) return;

	PawnController->SetShowMouseCursor(true);

	AIslesGameModeBase* GameMode = Cast<AIslesGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!ensure(GameMode != nullptr)) return;
	GameMode->GoldChanged.Broadcast();

	TArray<AActor*> Ships;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AShipBase::StaticClass(),Ships);
	if (Ships.Num()>0)
	{
		for (auto& Ship : Ships)
		{
			if (AShipBase* ShipClass = Cast<AShipBase>(Ship))
			{
				AddShipToFleet(ShipClass);
			}
		}
	}
}

void AIslesPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AIslesPlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// make able Q and E (pawn yaw control rotation)
	bUseControllerRotationYaw = true;


	// Bind IMC and IA
	if (ULocalPlayer* LocalPlayer = GetWorld()->GetFirstPlayerController()->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			if (IMC_IslePlayer)
			{
				InputSubsystem->ClearAllMappings();
				InputSubsystem->AddMappingContext(IMC_IslePlayer, 0);

				if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
				{
					Input->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &AIslesPlayerBase::MoveForward);
					Input->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &AIslesPlayerBase::MoveRight);
					Input->BindAction(IA_RotateCamera, ETriggerEvent::Triggered, this, &AIslesPlayerBase::RotateCamera);
					Input->BindAction(IA_Mouse, ETriggerEvent::Started, this, &AIslesPlayerBase::MouseControl);
					Input->BindAction(IA_MouseWheel, ETriggerEvent::Triggered, this, &AIslesPlayerBase::MouseZoom);
				}
			}
		}
	}
}

void AIslesPlayerBase::CallIsleInfo(AIsleBase* Isle)
{
	if (!IsleInfo)
	{
		IsleInfo = CreateWidget<UIsleInfo>(GetWorld(), IsleInfoClass);
		IsleInfo->AddToViewport();
	}
	
	IsleInfo->UpdateItemList(Isle);
	IsleInfo->SetVisibility(ESlateVisibility::Visible);
}


/* controls
 */
void AIslesPlayerBase::MoveForward(const FInputActionValue& Value)
{
	float InputVal = Value.Get<float>();
	MovementComponent->AddInputVector(GetActorForwardVector() * InputVal);
}

void AIslesPlayerBase::MoveRight(const FInputActionValue& Value)
{
	float InputVal = Value.Get<float>();
	MovementComponent->AddInputVector(GetActorRightVector() * InputVal);
}

void AIslesPlayerBase::RotateCamera(const FInputActionValue& Value)
{
	AddControllerYawInput(Value.Get<float>());
}

void AIslesPlayerBase::MouseControl(const FInputActionValue& Value)
{
	if (auto Hit = PawnController->GetHitResultUnderCursor(ECC_Visibility, true, MouseHit))
	{
		DrawDebugSphere(GetWorld(), MouseHit.Location, 10.f, 10, FColor::Red, false, 3);
		if (AShipBase* Ship = Cast<AShipBase>(MouseHit.GetActor()); Ship)
		{
			 ShipInFocus = Ship;
			// UE_LOG(LogTemp, Warning, TEXT("Ship in focus: %s"), *ShipInFocus->GetName())
		}
		else if (AIsleBase* Isle = Cast<AIsleBase>(MouseHit.GetActor()); Isle && Isle->bIsPort)
		{
			UE_LOG(LogTemp, Warning, TEXT("Isle clicked: %s"), *MouseHit.GetActor()->GetName());

			// if (ShipInFocus && !ShipInFocus->bIsAutoPathing)
			// {
			// 	TArray<UActorComponent*> FoundPortPoints;
			// 	FoundPortPoints = Isle->GetComponentsByTag(USceneComponent::StaticClass(),TEXT("PortPoint"));
			// 	if (!FoundPortPoints.IsEmpty())
			// 	{
			// 		if (auto Point = Cast<USceneComponent>(FoundPortPoints[0]))
			// 		{
			// 			ShipInFocus->ClickToMove(Point->GetComponentLocation());
			//
			// 			Isle->MooreShip(ShipInFocus);
			// 			ShipInFocus->LastPortVisited = Isle;
			// 		}
			// 	}
			// 	else
			// 	{
			// 		UE_LOG(LogTemp, Warning, TEXT("Ship %s cannot found for %s port"), *ShipInFocus->GetName(),
			// 		       *Isle->GetName());
			// 	}
			//}
			CallIsleInfo(Isle);
		}
		else
		{
			if (IsleInfo)
			{
				IsleInfo->SetVisibility(ESlateVisibility::Hidden);
			}

			// if (ShipInFocus && !ShipInFocus->bIsAutoPathing)
			// {
			// 	UE_LOG(LogTemp, Warning, TEXT("Ship in focus: %s"), *ShipInFocus->GetName())
			// 	ShipInFocus->ClickToMove(MouseHit.Location);
			// }
		}
	}
}

void AIslesPlayerBase::MouseZoom(const FInputActionValue& Value)
{
	// If just prevents fmath::clamp calculation in case min/max distance already setted
	if (SpringArm->TargetArmLength > 899 && SpringArm->TargetArmLength < 2401)
	{
		// Interp looks more smooth for me. I think there is better way to make camera zoom with mouse wheel, but...
		float Target = FMath::Clamp(SpringArm->TargetArmLength + Value.Get<float>() * 200, 900, 2401);
		SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, Target, GetWorld()->GetDeltaSeconds(),
		                                              10.f);
	}
}



void AIslesPlayerBase::AddShipToFleet(AShipBase* Ship)
{
		if (Ship)
		{
			PlayerShips.Add(Ship);
			UpdateFleetNumbers();
		}
}


void AIslesPlayerBase::UpdateFleetNumbers()
{
	int FreeShip = 0;
	int BotShip = 0;
	for (auto& Ship : PlayerShips)
	{
		if (Ship->bIsAutoPathing)	
		{
			BotShip++;
		} else
		{
			FreeShip++;
		}
	}

	if ( AIslePlayerController* PlayerController = Cast<AIslePlayerController>(GetController()))
	{
		PlayerController->DaysCountWidget->FreeShips = FreeShip;
		PlayerController->DaysCountWidget->BotShips = BotShip;
	}

}