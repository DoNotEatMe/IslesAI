// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Ship/AIShipController.h"

#include "IslesGameModeBase.h"
#include "IslesPlayerbase.h"
#include "NavigationSystem.h"
#include "ShipBase.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Objects/Isle/IsleBase.h"
#include "UI/IsleInfo.h"
#include "UI/LoadUnload.h"


AAIShipController::AAIShipController()
{
}

void AAIShipController::BeginPlay()
{
	Super::BeginPlay();
	IsleGameMode = Cast<AIslesGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!ensure(IsleGameMode != nullptr)) return;

	NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!ensure(NavSystem != nullptr)) return;
}


void AAIShipController::ClickToMove(AIsleBase* TargetIsle)
{
	if (!ensure(TargetIsle != nullptr)) return;

	
	
	if ( ControlledPawn && ControlledPawn->LastPortVisited)
	{
		ControlledPawn->bIsMoored = false;
		ControlledPawn->LastPortVisited->UnMoorShip(ControlledPawn);
	} 

	UActorComponent* PortPoint = TargetIsle->FindComponentByTag(USceneComponent::StaticClass(),TEXT("PortPoint"));
	if (!ensure(PortPoint != nullptr)) return;

	USceneComponent* Port = Cast<USceneComponent>(PortPoint);
	if (!ensure(Port != nullptr)) return;

	ControlledPawn->NextPortToVisit = TargetIsle;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(Port->GetComponentLocation());
	MoveRequest.SetUsePathfinding(true);
	MoveRequest.SetAcceptanceRadius(5.0f);

	FNavPathSharedPtr NavPath;
	FPathFollowingRequestResult Result = this->MoveTo(MoveRequest, &NavPath);

	AIslesPlayerBase* Player = Cast<AIslesPlayerBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!ensure(Player != nullptr)) return;
	if (Player->IsleInfo->GetVisibility() == ESlateVisibility::Visible)
	{
		Player->IsleInfo->UpdateItemList(Player->IsleInfo->CurrentIsle);
	}
	
	if (Result.Code == EPathFollowingRequestResult::RequestSuccessful && NavPath.IsValid())
	{
		const TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();

		for (const FNavPathPoint& Point : PathPoints)
		{
			DrawDebugSphere(GetWorld(), Point.Location, 50.0f, 12, FColor::Blue, false, 10.0f);
		}
	}
}

void AAIShipController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	switch (Result.Code.GetValue())
	{
	case EPathFollowingResult::Success:

		UE_LOG(LogTemp, Warning, TEXT("AAIShipController::OnMoveCompleted Path following succeeded."));

		ControlledPawn->LastPortVisited = ControlledPawn->NextPortToVisit;
		ControlledPawn->NextPortToVisit = nullptr;
	
		ControlledPawn->bIsMoored;
		ControlledPawn->LastPortVisited->MooreShip(ControlledPawn);

	if (!ControlledPawn->bIsAutoPathing)
	{
		// if (ControlledPawn->LastPortVisited && FVector::Dist(ControlledPawn->LastPortVisited->GetActorLocation(),
		//                   ControlledPawn->GetActorLocation()) < 200.f)
		// {
		// 	//AIslesPlayerBase* Player = Cast<AIslesPlayerBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		// 	//if (!ensure(Player != nullptr)) return;
		// 	//Player->CallIsleInfo(ControlledPawn->LastPortVisited);			
		// }
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Sell timer goes brr"));
		ControlledPawn->ProgressWidget->bBuy = false;
		ControlledPawn->ProgressWidget->Progress = 0;
		ControlledPawn->ProgressWidget->SetVisibility(ESlateVisibility::Visible);
		GetWorld()->GetTimerManager().SetTimer(SellTimerHandle,this,&AAIShipController::SellTimer,0.1,true);
	}
		break;

	case EPathFollowingResult::Blocked:
		UE_LOG(LogTemp, Warning, TEXT("AAIShipController::OnMoveCompleted Path following was blocked."));
		break;
	case EPathFollowingResult::OffPath:
		UE_LOG(LogTemp, Warning, TEXT("AAIShipController::OnMoveCompleted Off the path."));
		break;
	case EPathFollowingResult::Aborted:
		UE_LOG(LogTemp, Warning, TEXT("AAIShipController::OnMoveCompleted Path following was aborted."));
		break;
	case EPathFollowingResult::Invalid:
		UE_LOG(LogTemp, Warning, TEXT("AAIShipController::OnMoveCompleted Invalid path following result."));
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("AAIShipController::OnMoveCompleted Unknown path following result: %d"),
		       Result.Code.GetValue());
		break;
	}
}


void AAIShipController::SellTimer()
{
	ElapsedTime += 0.1f;
	ControlledPawn->ProgressWidget->Progress = ElapsedTime / 2;
	if (ElapsedTime >= 2)
	{
		ControlledPawn->ProgressWidget->SetVisibility(ESlateVisibility::Hidden);
		ControlledPawn->AutoSell();
		ControlledPawn->RunAutoTrade();
		GetWorld()->GetTimerManager().ClearTimer(SellTimerHandle);
		ElapsedTime = 0;
	}
}


bool AAIShipController::GetProjectedNavPoints(const FVector& StartPoint, const FVector& EndPoint)
{
	if (StartPoint == FVector::ZeroVector || EndPoint == FVector::ZeroVector)
	{
		UE_LOG(LogTemp, Error, TEXT("AAIShipController::GetProjectedNavPoints (StartPoint || EndPoint) == FVector::ZeroVector"));
		return false;
	}
	
	if(!ensure(NavSystem != nullptr))
	return false;

	FNavLocation StartPointLocation;
	FNavLocation EndPointLocation;

	bool bProjectedStartPoint = NavSystem->ProjectPointToNavigation(StartPoint, StartPointLocation);
	bool bProjectedEndPoint = NavSystem->ProjectPointToNavigation(EndPoint, EndPointLocation);

	if (bProjectedStartPoint && bProjectedEndPoint)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Both project points are succeed check for NavMesh"));
		return true;
	}

	if (!bProjectedStartPoint)
	{
		UE_LOG(LogTemp, Error,
		       TEXT("AAIShipController::GetProjectedNavPoints StartPoint failed project point to NavMesh"));
	}
	if (!bProjectedEndPoint)
	{
		UE_LOG(LogTemp, Error,
		       TEXT("AAIShipController::GetProjectedNavPoints EndPoint failed project point to NavMesh"));
	}

	return false;
}


