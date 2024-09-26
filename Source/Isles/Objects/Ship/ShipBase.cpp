// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Ship/ShipBase.h"
#include "IslesGameModeBase.h"
#include "AIShipController.h"
#include "IslesPlayerbase.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Objects/Isle/IsleBase.h"
#include "UI/LoadUnload.h"

// Sets default values
AShipBase::AShipBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = Health_Max;
	Weight = 0;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	RootComponent = BoxCollision;
	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	ShipMesh->SetupAttachment(RootComponent);

	ProgressBar = CreateDefaultSubobject<UWidgetComponent>("WidgetComponent");
	ProgressBar->SetupAttachment(ShipMesh);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnComponent"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIShipController::StaticClass();
}

// Called when the game starts or when spawned
void AShipBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentGoodsInfo.Empty();

	IsleGameMode = Cast<AIslesGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!ensure(IsleGameMode != nullptr)) return;
	IsleGameMode->OnDayPassed.AddDynamic(this, &ThisClass::DisableWaiting);

	AIController = Cast<AAIShipController>(GetController());
	if (!ensure(AIController != nullptr)) return;

	AIController->ControlledPawn = this;

	Player = Cast<AIslesPlayerBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!ensure(Player != nullptr)) return;

	ProgressWidget = Cast<ULoadUnload>(ProgressBar->GetWidget());
	if (!ensure(ProgressWidget != nullptr)) return;
}

// Called every frame
void AShipBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementComponent)
	{
		FVector Velocity = MovementComponent->Velocity;
		if (!Velocity.IsZero())
		{
			FRotator NewRotation = Velocity.Rotation();
			SetActorRotation(FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 5.0f));
		}
	}
}

bool AShipBase::AddItems(FName ItemName, int Quantity, float Price, float ItemWeight)
{
	UE_LOG(LogTemp, Warning, TEXT("TO BUY Name: %s, Stock: %d, Weight: %f, Price: %.2f"), *ItemName.ToString(),Quantity,ItemWeight,Price)
	if (Quantity * ItemWeight + Weight <= Weight_Max)
	{
		//UE_LOG(LogTemp,Warning,TEXT("Weight chek ok"));
		TSharedPtr<FGoods> FoundItem = FindItem(ItemName);
		if (FoundItem.IsValid())
		{
			FoundItem->Stock_Current += Quantity;
			FoundItem->Weight += FMath::RoundToFloat(ItemWeight * Quantity * 100.0f) / 100.0f;
			FoundItem->BasePrice += FMath::RoundToFloat(Quantity * Price * 100.0f) / 100.0f;
			
			UE_LOG(LogTemp, Warning, TEXT("BOUGHT Name: %s, Stock: %d, Weight: %f, Price: %.2f"), *FoundItem->ItemName.ToString(),FoundItem->Stock_Current,FoundItem->Weight,FoundItem->BasePrice)
		}
		else
		{
			TSharedPtr<FGoods> StructToAdd = MakeShareable(new FGoods());
			StructToAdd->ItemName = ItemName;
			StructToAdd->Stock_Current = Quantity;
			StructToAdd->BasePrice = FMath::RoundToFloat(Quantity * Price * 100.0f) / 100.0f;
			StructToAdd->Weight = FMath::RoundToFloat(ItemWeight * Quantity * 100.0f) / 100.0f;

			UE_LOG(LogTemp, Warning, TEXT("BOUGHT Name: %s, Stock: %d, Weight: %f, Price: %.2f"), *StructToAdd->ItemName.ToString(),StructToAdd->Stock_Current,StructToAdd->Weight,StructToAdd->BasePrice)

			CurrentGoodsInfo.Add(StructToAdd);
		}

		Weight += Quantity * ItemWeight;

		// UE_LOG(LogTemp, Error, TEXT("--AShipBase::AddItems--"));
		// UE_LOG(LogTemp, Warning, TEXT("New weight: %f"), Weight);
		// for (auto& a : CurrentGoodsInfo)
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("Name: %s, Stock: %d, Weight: %f, Price: %f"), *a->ItemName.ToString(),
		// 	       a->Stock_Current,
		// 	       a->Weight, a->BasePrice);
		// }
		// UE_LOG(LogTemp, Error, TEXT("-----------------------"));

		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("AShipBase::AddItems adding Item failed"));

	return false;
}

// look for debug here
bool AShipBase::RemoveItems(FName ItemName, int Quantity, float ItemWeight)
{
	UE_LOG(LogTemp, Warning, TEXT("TO SELL Name: %s, Stock: %d, Weight: %.2f"), *ItemName.ToString(),Quantity,ItemWeight);
	
	TSharedPtr<FGoods> FoundItem = FindItem(ItemName);
	if (FoundItem.IsValid())
	{
		if (FoundItem->Stock_Current >= Quantity)
		{
			UE_LOG(LogTemp, Warning, TEXT("PRESELL Name: %s, CurrentStock: %d, Weight: %f, Price: %.2f"),*FoundItem->ItemName.ToString(),FoundItem->Stock_Current,FoundItem->Weight,FoundItem->BasePrice);
			UE_LOG(LogTemp, Warning, TEXT("SELLING Name: %s, Quantity: %d, Weight: %f, Price: %.2f"), *FoundItem->ItemName.ToString(),Quantity,Quantity * ItemWeight,((FoundItem->BasePrice / FoundItem->Stock_Current) *
				Quantity))

			FoundItem->BasePrice = FMath::Abs( FoundItem->BasePrice - FoundItem->BasePrice / FoundItem->Stock_Current *	Quantity);
			FoundItem->Stock_Current -= Quantity;
			FoundItem->Weight -= Quantity * ItemWeight;
			Weight -= Quantity * ItemWeight;

			UE_LOG(LogTemp, Warning, TEXT("LeftAfterSell Name: %s, CurrentStock: %d, Weight: %f, Price: %.2f"),*FoundItem->ItemName.ToString(),FoundItem->Stock_Current,FoundItem->Weight,FoundItem->BasePrice);
			
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AShipBase::RemoveItems !(FoundItem->Stock_Current >= Quantity)"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AShipBase::RemoveItems !FoundItem.IsValid()"));
		return false;
	}

	return false;
}

TSharedPtr<FGoods> AShipBase::FindItem(FName ItemName)
{
	for (const auto& Item : CurrentGoodsInfo)
	{
		if (Item->ItemName == ItemName)
		{
			return Item;
		}
	}
	return nullptr;
}


// debug purposes
int AShipBase::GetQuantity(FName ItemName)
{
	auto Item = FindItem(ItemName);
	if (Item.IsValid())
	{
		return Item->Stock_Current;
	}
	return INT_MIN;
}

void AShipBase::RunAutoTrade()
{
	bIsAutoPathing = true;
	Player->UpdateFleetNumbers();
	if (bIsMoored)
	{
		switch (AutoBuy())
		{
		case EAutoBuyResponse::Success:

			if (bBought)
			{
				UE_LOG(LogTemp, Warning, TEXT("Buy timer goes brr"));
				ProgressWidget->bBuy = true;
				ProgressWidget->Progress = 0;
				ProgressWidget->SetVisibility(ESlateVisibility::Visible);
				// for better progress bar i think need to refactor autobuy response logic 
				GetWorld()->GetTimerManager().SetTimer(BuySellTimerHandle, this, &ThisClass::BuyTimer, 0.1f, true);
				UE_LOG(LogTemp, Warning, TEXT("AutoBuy() Success"));
			}else
			{
				GetWorld()->GetTimerManager().SetTimer(WaitingNewDayHandle, this, &AShipBase::WaitingForNewDay, 0.1f,true);
			}
			break;

		case EAutoBuyResponse::BadProductRating:
			UE_LOG(LogTemp, Warning, TEXT("BadProductRating, waiting new day"));
			//wait for next day?
			GetWorld()->GetTimerManager().SetTimer(WaitingNewDayHandle, this, &AShipBase::WaitingForNewDay, 0.1f,true);
			break;

		case EAutoBuyResponse::Failed:
			UE_LOG(LogTemp, Error, TEXT("Failed"));
			break;

		default:
			break;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AShipBase::RunAutoTrade expect Ship->bIsMoored == true"));
	}
}

void AShipBase::BuyTimer()
{
	ElapsedTime += 0.1f;
	ProgressWidget->Progress = ElapsedTime / 2;

	if (ElapsedTime >= 2)
	{
		
		ElapsedTime = 0;
		ProgressWidget->SetVisibility(ESlateVisibility::Hidden);
		
		if (AIsleBase* BestIsle = GetBestIslandWithRating())
		{
			bBought = false;
			AIController->ClickToMove(BestIsle);
		} else
		{
			GetWorld()->GetTimerManager().SetTimer(WaitingNewDayHandle, this, &AShipBase::WaitingForNewDay, 0.1f,true);
		}
		
		GetWorld()->GetTimerManager().ClearTimer(BuySellTimerHandle);
		
	}
}


EAutoBuyResponse AShipBase::AutoBuy()
{
	if (!ensure(IsleGameMode != nullptr)) return EAutoBuyResponse::Failed;
	
	UDataTable* Data = IsleGameMode->Data;

	if (Data)
	{
		float FreeWeight = Weight_Max - Weight;

		TMap<FName, float> ProductsRating;

		// Forming ItemRating
		// Rating based on value for weight unit, so we will buy more valuable items first
		for (auto& Item : LastPortVisited->CurrentGoodsInfo)
		{
			const FString ContextString = FString::Printf(
				TEXT("Looking for rows in DT from %s"), *GetName());
			FGoods* Row = Data->FindRow<FGoods>(Item->ItemName, ContextString);

			if (Row)
			{
				if (Item->BasePrice < Row->BasePrice)
				{
					float Rating = (Row->BasePrice - Item->BasePrice) * (FreeWeight / Row->Weight);
					ProductsRating.Add(Item->ItemName, Rating);
				}
			}
			else
			{
				break;
			}
		}


		bool bProductsTooHeavy = true; // Weight checker

		while (FreeWeight > 0 && !ProductsRating.IsEmpty())
		{
			// FindBest
			FName BestItem = BestTrade(ProductsRating);

			if (BestItem.IsValid())
			{
				if (!ensure(LastPortVisited != nullptr)) return EAutoBuyResponse::Failed;

				auto PortItem = LastPortVisited->FindItem(BestItem);

				// Could buy at least 1
				//UE_LOG(LogTemp, Warning, TEXT("%s  %d > 0 && %f <= %f"), *PortItem->ItemName.ToString(),PortItem->Stock_Current, PortItem->Weight, FreeWeight);
				if (PortItem->Stock_Current > 0 && PortItem->Weight <= FreeWeight)
				{
					bProductsTooHeavy = false; // Yeah we could


					int QuantityToBuy = (PortItem->Weight * PortItem->Stock_Current <= FreeWeight)
						                    ? PortItem->Stock_Current
						                    : FMath::TruncToInt(FreeWeight / PortItem->Weight);

					// Enough gold?
					if (PortItem->BasePrice * QuantityToBuy > Player->Gold)
					{
						QuantityToBuy = FMath::TruncToInt(Player->Gold / PortItem->BasePrice);
					}

					LastPortVisited->BuyItems(this, BestItem, QuantityToBuy);

					FreeWeight = Weight_Max - Weight;
				}
				else
				{
					bBought = true;
				}

				ProductsRating.Remove(BestItem);
				bBought = true;
				
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AShipBase::AutoBuy !BestItem->IsValid()"));
				return EAutoBuyResponse::Failed;
			}
		}
		UE_LOG(LogTemp, Error, TEXT("AShipBase::AutoBuy on Isle: %s"), *LastPortVisited->GetName());
		GetCurrentStockInfo();

		if (Weight == Weight_Max)
		{
			bBought = true;
		}

		// If have free weight but products too heavy
		if (bProductsTooHeavy && !ProductsRating.IsEmpty())
		{
			//UE_LOG(LogTemp, Warning, TEXT("AShipBase::AutoBuy All remaining products are too heavy to fit"));
			return EAutoBuyResponse::Success;
		}
		//UE_LOG(LogTemp, Warning, TEXT("Looks like clear success"));
		return EAutoBuyResponse::Success;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AShipBase::AutoBuy Data not found"));
		return EAutoBuyResponse::Failed;
	}
}


// AutoSell/Buy loop in case if we have unit price < Isle unit price < Base unit price
// so we just sell/buy before ship unit price reaching isle unit price. Not critical, just ugly
void AShipBase::AutoSell()
{
	if (!ensure(IsleGameMode != nullptr)) return;

	UDataTable* Data = IsleGameMode->Data;
	if (!ensure(Data != nullptr)) return;

	// Week changed while we were in sea? -> update path
	// Case if Isle rating changed
	if (!CurrentGoodsInfo.IsEmpty() && !bWeekChanged)
	{
		for (auto Item : CurrentGoodsInfo)
		{
			const FString ContextString = FString::Printf(
				TEXT("Looking for rows in DT from %s"), *GetName());
			FGoods* Row = Data->FindRow<FGoods>(Item->ItemName, ContextString);

			auto PortItem = LastPortVisited->FindItem(Item->ItemName);

			// Item.Price = Cumulative price. so, 100/10 = 10 for unit <  11 item price in port -> let's buy
			if (Row && PortItem && Item->BasePrice / Item->Stock_Current < PortItem.Get()->BasePrice && Item->
				Stock_Current > 0)
			{
				Item->Stock_Current + PortItem->Stock_Current <= Row->Stock_Max
					? LastPortVisited->SellItems(this, Item->ItemName, Item->Stock_Current)
					: LastPortVisited->SellItems(this, Item->ItemName, Row->Stock_Max - PortItem->Stock_Current);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AShipBase::AutoSell CurrentGoodsInfo.IsEmpty()"));
	}
}


// Debug puproses
void AShipBase::GetCurrentStockInfo()
{
	UE_LOG(LogTemp, Error, TEXT("-------Current Ship Stock-------"));
	for (auto Item : CurrentGoodsInfo)
	{
		UE_LOG(LogTemp, Warning, TEXT("Name: %s, Stock: %d, Weight: %.2f, Price: %.2f"), *Item->ItemName.ToString(),
		       Item->Stock_Current,
		       Item->Weight, Item->BasePrice);
	}
	UE_LOG(LogTemp, Warning, TEXT("gold: %.2f "), Player->Gold);
	UE_LOG(LogTemp, Error, TEXT("-------------------------------"));
}

void AShipBase::WaitingForNewDay()
{
	if (bStopWait)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Stoping wait bStopWait = false // timer"));
		GetWorld()->GetTimerManager().ClearTimer(WaitingNewDayHandle);
		bStopWait = false;
		bBought = false;
		RunAutoTrade();
		
		
	}
}


void AShipBase::DisableWaiting()
{
	//UE_LOG(LogTemp, Warning, TEXT("Stoping wait bStopWait = true // dispatcher"));
	bStopWait = true;
}

AIsleBase* AShipBase::GetBestIslandWithRating()
{
	// if we are waiting and catch new week, we need to know current island rating
	TMap<AIsleBase*, float> IslesRating;

	AIsleBase* BestIsle = nullptr;
	float BestRating = 0;
	float Speed = MovementComponent->GetMaxSpeed();

	IslesRating.Empty();

	if (!IsleGameMode->PortIsles.IsEmpty())
	{
		for (auto& Port : IsleGameMode->PortIsles)
		{
			float Rating = 0;

			for (auto& Item : CurrentGoodsInfo)
			{
				auto Found = Port->FindItem(Item->ItemName);
				if (Found && Item->Stock_Current > 0 && Found->BasePrice > Item->BasePrice / Item->Stock_Current)
				{
					Rating += Found->BasePrice / (Item->BasePrice / Item->Stock_Current);
				}
			}

			float PathLength = PathsRating(Port);
			float PathSeconds = PathLength / Speed;

			//IsleGameMode->DayInSeconds
			if (PathSeconds >= IsleGameMode->DayInSeconds && PathSeconds < IsleGameMode->DayInSeconds * 14)
			{
				// just for hold case where current isle is best
				float DeltaRating = Rating / (PathSeconds == 0 ? 1 : PathSeconds);
				IslesRating.Add(Port, DeltaRating);

				if (DeltaRating > BestRating)
				{
					BestRating = DeltaRating;
					BestIsle = Port;
					// UE_LOG(LogTemp, Warning, TEXT("New Best Isle: %s, PathSeconds: %f, DeltaRating: %f"),
					//        *Port->GetName(), PathSeconds, DeltaRating);
				}
			} 
		}
	}

	// here was if !BestIsle { BestIsle = LastPortVisited } but it causing lots of other logistical questions

	
	UE_LOG(LogTemp, Warning, TEXT("%s"),
	       BestIsle
	       ? *FString::Printf(TEXT("Best Isle: %s with DeltaRating: %f"), *BestIsle->GetName(), BestRating)
	       : TEXT("No Best Isle found"));

	return BestIsle;
}

void AShipBase::GetBestIslandWithRating(TArray<TPair<AIsleBase*, float>>& SortedIsles)
{
	float Speed = MovementComponent->GetMaxSpeed();

	SortedIsles.Empty();

	if (!IsleGameMode->PortIsles.IsEmpty())
	{
		for (auto& Port : IsleGameMode->PortIsles)
		{
			float Rating = 0;

			for (auto& Item : CurrentGoodsInfo)
			{
				auto Found = Port->FindItem(Item->ItemName);
				if (Found && Item->Stock_Current > 0 && Found->BasePrice > Item->BasePrice / Item->Stock_Current)
				{
					Rating += Found->BasePrice / (Item->BasePrice / Item->Stock_Current);
				}
			}

			float PathLength = PathsRating(Port);
			float PathSeconds = PathLength / Speed;

			if (PathSeconds > IsleGameMode->DayInSeconds && PathSeconds < IsleGameMode->DayInSeconds * 14)
			{
				float DeltaRating = Rating / PathSeconds;
				SortedIsles.Add(TPair<AIsleBase*, float>(Port, DeltaRating));
			}
		}

		//UE_LOG(LogTemp, Warning, TEXT("Isles num: %d"), SortedIsles.Num());

		// Not clearly understand this, but...
		SortedIsles.Sort([](const TPair<AIsleBase*, float>& A, const TPair<AIsleBase*, float>& B)
		{
			return A.Value > B.Value;
		});
	}
}

bool AShipBase::GetIslesRating(TMap<AIsleBase*, float>& IslesRating)
{
	if (!IsleGameMode->PortIsles.IsEmpty())
	{
		for (auto& Port : IsleGameMode->PortIsles)
		{
			float Rating = 0;
			for (auto& Item : CurrentGoodsInfo)
			{
				auto Found = Port->FindItem(Item->ItemName);
				if (Found)
				{
					// Item base price is ++ price in ship ¯\_(ツ)_/¯
					if (Item->Stock_Current > 0 && Found->BasePrice > Item->BasePrice / Item->Stock_Current)
					{
						Rating += Found->BasePrice / (Item->BasePrice / Item->Stock_Current);
					}
				}
			}
			//DrawDebugSphere(GetWorld(), Port->GetActorLocation(), 100, 10, FColor::Red, false, 10);

			//UE_LOG(LogTemp, Warning, TEXT("Port: %s, Rating: %f"), *Port->GetName(), Rating);
			IslesRating.Add(Port, Rating);
		}
		return true;
	}
	UE_LOG(LogTemp, Error, TEXT("AShipBase::GetIslesRating !IsleGameMode->PortIsles.IsEmpty()"));
	return false;
}

void AShipBase::AddToFleet()
{
	Player->AddShipToFleet(this);
}


FName AShipBase::BestTrade(TMap<FName, float> ProductsRating)
{
	// Choosing best
	FName* BestItem = nullptr;
	float BestRating = -FLT_MAX;

	for (auto& Elem : ProductsRating)
	{
		if (Elem.Value > BestRating)
		{
			BestRating = Elem.Value;
			BestItem = &Elem.Key;
		}
	}
	return *BestItem;
}

AIsleBase* AShipBase::BestIsland(TMap<AIsleBase*, float>& IslesRating)
{
	AIsleBase* BestIsle = nullptr;

	float DeltaRating = 0;
	float Speed = MovementComponent->GetMaxSpeed();

	for (auto& Isle : IslesRating)
	{
		float PathLength = PathsRating(Isle.Key);

		float PathSeconds = PathLength / Speed;

		if (PathSeconds > IsleGameMode->DayInSeconds && PathSeconds < IsleGameMode->DayInSeconds * 14 && DeltaRating
			< Isle.Value / PathSeconds)
		{
			DeltaRating = Isle.Value / PathSeconds;
			// UE_LOG(LogTemp, Warning, TEXT(" New Best  --  Isle: %s, PathSeconds %f,DeltaRating %f, Isle.Value %f"),
			//        *Isle.Key->GetName(), PathSeconds, DeltaRating, Isle.Value);
			BestIsle = Isle.Key;
		}
	}


	if (!BestIsle && *IslesRating.Find(LastPortVisited) > 0)
	{
		BestIsle = LastPortVisited;
		// UE_LOG(LogTemp, Warning, TEXT("Last Island visited is best"));
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"),
	       BestIsle
	       ?*FString::Printf(TEXT("Isle: %s looks best"), *BestIsle->GetName())
	       :TEXT("No BestIsle Found"));

	return BestIsle;
}

float AShipBase::PathsRating(AIsleBase* TargetIsle)
{
	// UE_LOG(LogTemp, Warning, TEXT("Cheking %s"), *TargetIsle->GetName());

	UActorComponent* PortPoint = TargetIsle->FindComponentByTag(USceneComponent::StaticClass(),TEXT("PortPoint"));
	if (!ensure(PortPoint != nullptr)) return 0.f;
	USceneComponent* Port = Cast<USceneComponent>(PortPoint);
	if (!ensure(Port != nullptr)) return 0.f;

	FVector StartLocation = GetActorLocation();
	FVector EndLocation = TargetIsle->MooringPoint->GetComponentLocation();

	if (!AIController->GetProjectedNavPoints(StartLocation, EndLocation))
	{
		UE_LOG(LogTemp, Error, TEXT("AShipBase::PathsRating IController->GetProjectedNavPoints"));
	}

	UNavigationPath* Path = AIController->NavSystem->FindPathToLocationSynchronously(
		GetWorld(), StartLocation, EndLocation);

	if (Path->IsValid())
	{
		return Path->GetPathLength();
	}


	// UE_LOG(LogTemp, Warning, TEXT("AShipBase::PathsRating !Path->IsValid() %s"), TargetIsle != LastPortVisited
	//        ? *FString::Printf(
	// 	       TEXT("to %s"),
	// 	       *TargetIsle->GetName())
	//        : *FString::Printf(
	// 	       TEXT("but %s == %s"),
	// 	       *TargetIsle->GetName(),
	// 	       *LastPortVisited->GetName()));

	DrawDebugBox(GetWorld(), TargetIsle->GetActorLocation(), FVector(100.f, 100.f, 100.f), FColor::Red, false, 50);

	return 0.f;
}
