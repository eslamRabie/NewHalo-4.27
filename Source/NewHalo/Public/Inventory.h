// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "Components/ActorComponent.h"
#include "GameUI/NewHaloHUD.h"
#include "Inventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEWHALO_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AEquipment* GetEquipmentInSlot(int32 SlotIndex);

	void AddEquipmentInFirstEmptySlot(AEquipment* Equipment);

	void DropEquipmentFromSlot(int32 SlotIndex);
	
	
protected:
	
	
private:
	int32 MaxItemsCount;

	UPROPERTY()
	TMap<int32, AEquipment*> InventoryList;

	UPROPERTY()
	ANewHaloHUD* HUD;
	
};
