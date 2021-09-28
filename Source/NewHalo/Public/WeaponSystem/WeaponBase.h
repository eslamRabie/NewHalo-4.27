// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/Equippable.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Pickable.h"
#include "WeaponBase.generated.h"

class ANewHaloCharacter;
class USphereComponent;
class AWeaponProjectileBase;
class AWeaponMagazineBase;

UENUM()
enum class EWeaponType
{
	Pistol,
	Melee,
	AR,
	SimiAR,
	Sniper,
	Shotgun,
	RPG
};

UCLASS(Abstract)
class NEWHALO_API AWeaponBase : public AActor, public IPickable, public IEquippable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	UFUNCTION()
	virtual void Fire(FVector Direction);

	UFUNCTION()
	virtual void Reload();
	
	///////////////////////////////////
	///
	///
	//////////////////////////////////////

	/**
	 *  Begin Pickable Interface 
	 */

	virtual void Pick_Implementation(ANewHaloCharacter* Owner, int Amount) override;
	virtual void Drop_Implementation(int Amount) override;

	
	/**
	 * //End Pickable Interface
	 */

	///////////////////////////////////
	///
	//////////////////////////////////////
	
	/**
	 * Begin Equippable Interface  
	 */

	virtual void Equip_Implementation(FName SocketName) override;

	virtual void UnEquip_Implementation() override;
	
	/**
	 * End Equippable Interface
	 */
	
protected:

	UFUNCTION()
	void ShootingCoolDown();
	
	// Weapon Specs
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponSpecs)
	float Range;
	
public:
	float GetRange() const;
	float GetFireRate() const;
	float GetReLoadTime() const;
	bool IsIsAutomatic() const;
	EWeaponType GetWeaponType() const;
	ANewHaloCharacter* GetOwnerPlayer() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponSpecs)
	float FireRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponSpecs)
	float ReLoadTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponSpecs)
	bool bIsAutomatic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponSpecs)
	float WeaponDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponSpecs)
	EWeaponType WeaponType;	
	
	// Core Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponCore)
	USphereComponent* SceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponCore)
	USkeletalMeshComponent* WeaponSkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponCore)
	UParticleSystemComponent* ParticleSystemComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponCore)
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponCore)
	FName MagazineSocketName;



	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponCore)
	ANewHaloCharacter* OwnerPlayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponCore)
	TSubclassOf<class AWeaponMagazineBase> MagazineClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponCore)
	AWeaponMagazineBase* WeaponMagazine;
		
	

	// VFX/SFX 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponFX)
	UParticleSystem* FiringEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponFX)
	USoundBase* FiringSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponFX)
	USoundBase* EmptyMagazineSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponFX)
	USoundBase* ReloadSound;

	UPROPERTY()
	FName AttachedSocketName;
public:
	FName GetAttachedSocketName() const;
	void SetAttachedSocketName(FName InAttachedSocketName);
protected:
	bool bCanFire;

	FTimerHandle FireTimerHandle;
	
};
