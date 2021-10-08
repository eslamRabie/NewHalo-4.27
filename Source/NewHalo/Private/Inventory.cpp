// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

#include "Player/NewHaloCharacter.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	auto NOP = GetOwner()->GetNetOwningPlayer();
	if(!NOP)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant Get NetOwningPlayer!! in %s"), *GetName());
		return;
	}
	auto PC = NOP->GetPlayerController(GetWorld());
	if(!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant Get PlayerController!! in %s"), *GetName());
		return;
	}
	HUD = PC->GetHUD<ANewHaloHUD>();
	if(!HUD)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant Get HUD!! in %s"), *GetName());
		return;
	}

	for(int i = 1; i <= MaxItemsCount; i++)
	{
		InventoryList.Add(i, nullptr);
	}
	// ...
	
}


// Called every frame
void UInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

AEquipment* UInventory::GetEquipmentInSlot(int32 SlotIndex)
{
	return InventoryList[SlotIndex];
}

void UInventory::AddEquipmentInFirstEmptySlot(AEquipment* Equipment)
{
	for (auto KeyVal : InventoryList)
	{
		if(!KeyVal.Value)
		{
			KeyVal.Value = Equipment;
			auto Icon = Equipment->GetIcon();
			HUD->SetInventoryItemIcon(Icon, KeyVal.Key);
			return;
		}
	}
}

void UInventory::DropEquipmentFromSlot(int32 SlotIndex)
{
	InventoryList[SlotIndex] = nullptr;
}


