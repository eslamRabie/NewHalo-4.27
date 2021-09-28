// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Equippable.h"
#include "WeaponMagazineBase.generated.h"

UCLASS()
class NEWHALO_API AWeaponMagazineBase : public AEquipment
{
	GENERATED_BODY()
	
	public:	
	// Sets default values for this actor's properties
	AWeaponMagazineBase();

	protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	public:
	UFUNCTION()
	virtual void Reload(int32 InAmount, int32& OutAmount);

	virtual bool IsEmpty();

	virtual class AWeaponProjectileBase* GetProjectile();

	protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Magazine)
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Magazine)
	int32 CurrentAmmoCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Magazine)
	USkeletalMeshComponent* MagazineSkeletalMeshComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Magazine)
	TSubclassOf<class AWeaponProjectileBase> ProjectileClass;

	// todo create a pool of projectiles;
	TArray<AWeaponProjectileBase*> MagazineAmmo;

};


