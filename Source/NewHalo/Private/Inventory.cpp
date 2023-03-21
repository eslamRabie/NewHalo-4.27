// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

#include "Player/NewHaloCharacter.h"

UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 1; i <= MaxItemsCount; i++)
	{
		InventoryList.Add(i, nullptr);
	}
	
	auto NOP = GetOwner()->GetNetOwningPlayer();
	if (!NOP)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant Get NetOwningPlayer!! in %s"), *GetName());
		return;
	}
	auto PC = NOP->GetPlayerController(GetWorld());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant Get PlayerController!! in %s"), *GetName());
		return;
	}
	HUD = PC->GetHUD<ANewHaloHUD>();
	if (!HUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant Get HUD!! in %s"), *GetName());
		return;
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
		if (!KeyVal.Value)
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
