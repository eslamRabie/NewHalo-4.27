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

UENUM(Blueprintable, BlueprintType)
enum class EWeaponType: uint8
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	UFUNCTION()
	virtual void Fire(FVector Location, FRotator Rotation);

	UFUNCTION()
	virtual void Reload();


	UFUNCTION()
	FVector GetAmmo();
	
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
	

	
public:
	float GetRange() const;
	float GetFireRate() const;
	float GetReLoadTime() const;
	bool IsIsAutomatic() const;
	EWeaponType GetWeaponType() const;
	ANewHaloCharacter* GetOwnerPlayer() const;
	
protected:
	// Weapon Specs
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponSpecs)
	float Range;
	
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

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=WeaponFX)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponFX)
	UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponFX)
	UAnimMontage* ReloadAnimation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponFX)
	UParticleSystem* FiringEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponFX)
	USoundBase* EmptyMagazineSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponFX)
	USoundBase* ReloadSound;


	/// Design
	/// 

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=WeaponFX)
	UTexture2D* WeaponIcon;
public:
	UTexture2D* GetWeaponIcon() const;
protected:
	UPROPERTY()
	FName AttachedSocketName;
	
public:
	FName GetAttachedSocketName() const;
	void SetAttachedSocketName(FName InAttachedSocketName);
protected:
	bool bCanFire;
	bool bStopFiring;
public:
	void SetStopFiring(bool InbStopFiring);
protected:
	FTimerHandle FireTimerHandle;
	
};

inline void AWeaponBase::SetStopFiring(bool InbStopFiring)
{
	this->bStopFiring = InbStopFiring;
}
