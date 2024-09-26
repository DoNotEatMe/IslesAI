// Fill out your copyright notice in the Description page of Project Settings.


#include "IslesGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/Isle/IsleBase.h"
#include "Objects/Ship/ShipBase.h"
#include "Objects/Ship/AIShipController.h"

AIslesGameModeBase::AIslesGameModeBase()
{
	ConstructorHelpers::FObjectFinder<UDataTable> LookForTable(TEXT("/Game/Objects/Goods/DT_Goods.DT_Goods"));
	if (LookForTable.Succeeded())
	{
		Data = LookForTable.Object;
	}
	else { UE_LOG(LogTemp, Error, TEXT("GameMode LookForTable failed")); }
}

void AIslesGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 1d = 5s
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	World->GetTimerManager().SetTimer(DayCountTimerHandle, this, &AIslesGameModeBase::UpdateDays, DayInSeconds, true);

	GoldChanged.Broadcast();

	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIsleBase::StaticClass(), Isles);
	
	// Set Isles with port
	ChoosePorts();
}

void AIslesGameModeBase::UpdateDays()
{
	DaysCount++;
	UE_LOG(LogTemp, Warning, TEXT("day: %d"), DaysCount);
	// Broadcast event to listeners (Isles/Ships/...)
	OnDayPassed.Broadcast();
	if (DaysCount%7 == 0)
	{
		OnWeekPassed.Broadcast();
	}
}


void AIslesGameModeBase::ChoosePorts()
{
	int IslesLength = Isles.Num();

	if (IslesLength >= IslesWithPort)
	{
		// Shuffle array and add respawn

		int RandomInt = FMath::RandRange(0,IslesWithPort-1);
		
		
		for (int i = IslesLength - 1; i > 0; --i)
		{
			int j = FMath::RandRange(0, IslesLength - 1);

			Isles.Swap(i, j);
		}

		for (int i = 0; i < IslesWithPort; i++)
		{
			if (AIsleBase* Isle = Cast<AIsleBase>(Isles[i]))
			{
				Isle->SetPort();
				PortIsles.Add(Isle);
				
				// Random port spawn ship
				if( i == RandomInt)
				{
					TArray<UActorComponent*> ComponentsArray = Isle->GetComponentsByTag(USceneComponent::StaticClass(),TEXT("PortPoint"));
					if (ComponentsArray.Num() > 0)
					{
						USceneComponent* PortPoint = Cast<USceneComponent>(ComponentsArray[0]);

						FString BlueprintPath = TEXT("/Game/Objects/Ship/BP_Ship.BP_Ship_C");
						UClass* BlueprintClass = StaticLoadClass(UObject::StaticClass(), nullptr, *BlueprintPath);
						if (BlueprintClass)
						{
							FActorSpawnParameters SpawnParams;
							SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
							//FVector LocationtoSpawn = FVector(PortPoint->GetComponentLocation().X,PortPoint->GetComponentLocation().Y,PortPoint->GetComponentLocation().Z + 10);

							DrawDebugBox(GetWorld(),PortPoint->GetComponentLocation(),FVector(100.f,100.f,100.f),FColor::Red,false,10.f);
							
							AShipBase* Ship = GetWorld()->SpawnActor<AShipBase>(BlueprintClass,PortPoint->GetComponentLocation(), PortPoint->GetComponentRotation(),SpawnParams);
							Ship->LastPortVisited = Isle;
							Ship->bIsMoored;
							Isle->MooreShip(Ship);
							
						} else { UE_LOG(LogTemp,Error,TEXT("Failed to Spawn ship in AIslesGameModeBase::ChoosePorts()")); }
						
						
					}
					
					
					
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Isles To Spawn > Isles.Num() in AIslesGameModeBase::BeginPlay()"));
		UE_LOG(LogTemp, Error, TEXT("IslesWithPort Setted to Isles.Num()"));
		IslesWithPort = IslesLength;
		ChoosePorts();
	}
}
