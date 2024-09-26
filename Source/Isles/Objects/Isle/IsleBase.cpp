// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Isle/IsleBase.h"

#include "IslesGameModebase.h"
#include "IslesPlayerbase.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/LowLevelTestAdapter.h"
#include "Objects/Ship/ShipBase.h"

// Sets default values
AIsleBase::AIsleBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	IsleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IsleMesh"));
	RootComponent = IsleMesh;

	PortMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Port"));
	PortMesh->SetupAttachment(IsleMesh);
	PortMesh->SetVisibility(false, true);

	MooringPoint = CreateDefaultSubobject<USceneComponent>(TEXT("MooringPoint"));
	MooringPoint->SetupAttachment(PortMesh);
	MooringPoint->ComponentTags.Add("PortPoint");
}

// Called when the game starts or when spawned
void AIsleBase::BeginPlay()
{
	Super::BeginPlay();

	SetActorRotation(FRotator(0.f, FMath::RandRange(-180, 180), 0.f));
}

// Called every frame
void AIsleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIsleBase::SetPort()
{
	bIsPort = true;
	PortMesh->SetVisibility(true, true);

	IsleMesh->SetMaterial(0, PortMaterial);
	PortMesh->SetMaterial(0, PortMaterial);

	GameModePtr = Cast<AIslesGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!ensure(GameModePtr != nullptr)) return;

	// Listen to dispatcher for day change only for ports
	GameModePtr->OnDayPassed.AddDynamic(this, &AIsleBase::UpdateDay);

	// Basic goods setup
	UpdateGoods();
}

void AIsleBase::SellItems(AShipBase* Ship, FName ItemName, int Quantity)
{
	AIslesPlayerBase* Player = Cast<AIslesPlayerBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!ensure(Player != nullptr)) return;

	if (Ship->bIsMoored)
	{
		TSharedPtr<FGoods> FoundItem = FindItem(ItemName);

		if (FoundItem.IsValid())
		{
			if (FoundItem->Stock_Current + Quantity <= FoundItem->Stock_Max)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Selling Qunt = %d"),Quantity);
				if (Ship->RemoveItems(FoundItem->ItemName, Quantity, FoundItem->Weight))
				{
					
					FoundItem->Stock_Current += Quantity;
					Player->Gold += FoundItem->BasePrice * Quantity;
					
					GameModePtr->GoldChanged.Broadcast();
					GameModePtr->OnProductBuySellUpdate.Broadcast();
				}else
				{
					UE_LOG(LogTemp, Error, TEXT("AIsleBase::SellItems RemoveItems() == false"));
				}
			}
			else { UE_LOG(LogTemp, Error, TEXT("!FoundItem.IsValid() AIsleBase::SellItems")); }
		}
	}

	UE_LOG(LogTemp, Error, TEXT("AIsleBase::SellItems on Isle: %s"), *this->GetName());
	Ship->GetCurrentStockInfo();
}

void AIsleBase::BuyItems(AShipBase* Ship, FName ItemName, int Quantity)
{
	AIslesPlayerBase* Player = Cast<AIslesPlayerBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!ensure(Player != nullptr)) return;


	TSharedPtr<FGoods> FoundItem = FindItem(ItemName);
	if (FoundItem.IsValid())
	{
		if (FoundItem->Stock_Current >= Quantity)
		{
			float FinalPrice = FoundItem->BasePrice * Quantity;
			if (Player->Gold >= FinalPrice)
			{
				if (Ship->AddItems(FoundItem->ItemName, Quantity, FoundItem->BasePrice, FoundItem->Weight))
				{
					FoundItem->Stock_Current -= Quantity;
					//UE_LOG(LogTemp, Warning,TEXT("Spend: %f on %s"), FoundItem->BasePrice * Quantity,*FoundItem->ItemName.ToString());
					Player->Gold -= FinalPrice;
					GameModePtr->GoldChanged.Broadcast();
					GameModePtr->OnProductBuySellUpdate.Broadcast();
				}
			}
		}
	}
	else { UE_LOG(LogTemp, Error, TEXT("!FoundItem.IsValid() AIsleBase::BuyItems")); }

}

void AIsleBase::MooreShip(AShipBase* Ship)
{
	Ship->bIsMoored = true;
	MooredShips.Add(Ship);
}

void AIsleBase::UnMoorShip(AShipBase* Ship)
{
	Ship->bIsMoored = false;
	MooredShips.Remove(Ship);
}

void AIsleBase::UpdateDay()
{
	WeekCount++;

	if (WeekCount % 7 == 0)
	{
		// Clearing Current Goods info
		CurrentGoodsInfo.Empty();

		// UpdatingGoods + Setting New Prices
		UpdateGoods();

		WeekCount = 0;
	}
	else
	{
		// Every day changing current stock
		UpdateStock();
	}
}

void AIsleBase::UpdateGoods()
{
	UDataTable* Data = GameModePtr->Data;

	// Choose Goods to be sold on this day and write them up
	if (Data)
	{
		if (TArray<FName> RowNames = Data->GetRowNames(); RowNames.Num() > 0)
		{
			const FString ContextString = FString::Printf(TEXT("Looking for rows in DT from %s"), *GetName());
			for (const auto& RowName : RowNames)
			{
				const FGoods* Row = Data->FindRow<FGoods>(RowName, ContextString);

				if (Row)
				{
					// First time of usage TSharedPtr, not sure how it works at the moment, but understand problem
					// SharedPtr was made in case there were random (randbool) for each update, so was necessary i think
					TSharedPtr<FGoods> StructToAdd = MakeShareable(new FGoods());
					StructToAdd->ItemName = Row->ItemName;
					StructToAdd->Stock_Mim = Row->Stock_Mim;
					StructToAdd->Stock_Max = Row->Stock_Max;
					StructToAdd->Stock_Current = FMath::RandRange(StructToAdd->Stock_Mim, StructToAdd->Stock_Max);
					StructToAdd->BasePrice = Row->BasePrice;
					StructToAdd->Weight = Row->Weight;

					CurrentGoodsInfo.Add(StructToAdd);
				}
			}
			SetNewPrices();
		}
	}
}

TSharedPtr<FGoods> AIsleBase::FindItem(FName ItemName)
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

void AIsleBase::SetNewPrices()
{
	for (const auto& Item : CurrentGoodsInfo)
	{
		Item->BasePrice *= FMath::RandRange(.7, 1.3);
		Item->BasePrice = FMath::RoundToFloat(Item->BasePrice * 100.0f) / 100.0f;
		//UE_LOG(LogTemp, Warning, TEXT("Isle: %s,  %s new price: %f "), *this->GetName(), *Item->ItemName.ToString(), Item->BasePrice)
	}
}

void AIsleBase::UpdateStock()
{
	for (const auto& Item : CurrentGoodsInfo)
	{
		Item->Stock_Current = FMath::RandRange(Item->Stock_Mim, Item->Stock_Max);
		//UE_LOG(LogTemp, Warning, TEXT("Isle: %s,  %s new stock: %d "), *this->GetName(), *Item->ItemName.ToString(), Item->Stock_Current);
	}
}
