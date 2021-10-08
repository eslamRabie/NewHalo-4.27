// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Equippable.h"
#include "Interfaces/Pickable.h"
#include "Equipment.generated.h"

UCLASS()
class NEWHALO_API AEquipment : public AActor, public IEquippable,public IPickable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEquipment();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

/// Use Equipment
	virtual void Use();
	virtual void StopUsing();
	

// Start IEquippable interface
	virtual void Equip_Implementation(FName InSocketName) override;
	virtual void EquipInDesiredSocket_Implementation() override;
	virtual void UnEquip_Implementation() override;
// End IEquippable interface 

	
/// Start IPickable interface
	virtual void Pick_Implementation(ANewHaloCharacter* PickingPlayer, int Amount) override;
	virtual void Drop_Implementation(int Amount) override;
/// End IPickable interface

	
protected:	

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UStaticMeshComponent* WorldMeshComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess))
	UStaticMeshComponent* EquippedMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Equipment)
	FName SocketName;
	UPROPERTY()
	ANewHaloCharacter* CurrentOwner;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Equipment)
	UTexture2D* Icon;
	
public:
	FName GetSocketName() const;
	UTexture2D* GetIcon() const;



};
