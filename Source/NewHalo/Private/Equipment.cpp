// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment.h"

#include "Inventory.h"
#include "Player/NewHaloCharacter.h"

// Sets default values
AEquipment::AEquipment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WorldMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WorldStaticMeshComponent"));
	WorldMeshComponent->SetupAttachment(RootComponent);

	EquippedMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquippedStaticMeshComponent"));
	EquippedMeshComponent->SetupAttachment(RootComponent);

	EquippedMeshComponent->SetHiddenInGame(true, true);
}

// Called when the game starts or when spawned
void AEquipment::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEquipment::Use()
{
	
}

void AEquipment::StopUsing()
{
	
}

void AEquipment::Equip_Implementation(FName InSocketName)
{
	IEquippable::Equip_Implementation(InSocketName);
	auto SocketMesh1P = Cast<USkeletalMeshComponent>(GetDefaultSubobjectByName("CharacterMesh1P"));
	if(!SocketMesh1P)
	{
		return;
	}
	EquippedMeshComponent->AttachToComponent(SocketMesh1P, FAttachmentTransformRules::SnapToTargetIncludingScale, InSocketName);
	EquippedMeshComponent->SetHiddenInGame(false, true);
}

void AEquipment::EquipInDesiredSocket_Implementation()
{
	IEquippable::EquipInDesiredSocket_Implementation();
	Equip_Implementation(SocketName);
}

void AEquipment::UnEquip_Implementation()
{
	IEquippable::UnEquip_Implementation();
	EquippedMeshComponent->SetHiddenInGame(true, true);
	EquippedMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
}


void AEquipment::Pick_Implementation(ANewHaloCharacter* PickingPlayer, int Amount)
{
	IPickable::Pick_Implementation(PickingPlayer, Amount);
	if(!PickingPlayer)
	{
		return;
	}
	CurrentOwner = PickingPlayer;
	const auto Inventory = Cast<UInventory>(PickingPlayer->GetComponentByClass(UInventory::StaticClass()));
	if(!Inventory)
	{
		return;
	}
	WorldMeshComponent->SetHiddenInGame(true, true);
	Inventory->AddEquipmentInFirstEmptySlot(this);
}

void AEquipment::Drop_Implementation(int Amount)
{
	IPickable::Drop_Implementation(Amount);
	EquippedMeshComponent->SetHiddenInGame(true, true);
	WorldMeshComponent->SetHiddenInGame(false, true);
}

FName AEquipment::GetSocketName() const
{
	return SocketName;
}

UTexture2D* AEquipment::GetIcon() const
{
	return Icon;
}


