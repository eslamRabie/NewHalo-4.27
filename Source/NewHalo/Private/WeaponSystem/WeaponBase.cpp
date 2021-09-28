// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSystem/WeaponBase.h"

#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "WeaponSystem/WeaponMagazineBase.h"
#include "WeaponSystem/WeaponProjectileBase.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;\
	bCanFire = true;
	
	SceneComponent = CreateDefaultSubobject<USphereComponent>(TEXT("DefaultComponent"));
	RootComponent = SceneComponent;

	SceneComponent->SetCollisionProfileName("OverlapAll");

	MagazineSocketName = TEXT("MagSocket");
	MuzzleSocketName = TEXT("MuzzleSocket");
	
	WeaponSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponSkeletalMeshComponent->SetupAttachment(RootComponent);

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireVFX"));
	ParticleSystemComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	WeaponMagazine = GetWorld()->SpawnActor<AWeaponMagazineBase>(MagazineClass, FVector::ZeroVector, FRotator::ZeroRotator);
	WeaponMagazine->AttachToComponent(WeaponSkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, MagazineSocketName);	
	ParticleSystemComponent->Deactivate();
	ParticleSystemComponent->Template = FiringEffect;
}

void AWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBase::Fire(FVector Direction)
{
	if(!bCanFire)
	{
		return;
	}
	if(!WeaponMagazine)
	{
		// Shouldn't exist
		return;
	}
	if(WeaponMagazine->IsEmpty())
	{
		// Play Empty Magazine Sound
		return;
	}
	

	auto Projectile = WeaponMagazine->GetProjectile();

	if(Projectile)
	{
		bCanFire = false;
		ParticleSystemComponent->Activate();
		Projectile->OnShoot(OwnerPlayer, Direction, Range, WeaponDamage);
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeaponBase::ShootingCoolDown, FireRate, false);
	}
	
	
}

void AWeaponBase::Reload()
{
	int32 X;
	WeaponMagazine->Reload(40, X);
}

void AWeaponBase::Pick_Implementation(ANewHaloCharacter* PickingPlayer, int Amount)
{
	IPickable::Pick_Implementation(PickingPlayer, Amount);
	OwnerPlayer = PickingPlayer;
}

void AWeaponBase::Drop_Implementation(int Amount)
{
	IPickable::Drop_Implementation(Amount);
	OwnerPlayer = nullptr;
}

void AWeaponBase::Equip_Implementation(FName SocketName)
{
	IEquippable::Equip_Implementation(SocketName);
}

void AWeaponBase::UnEquip_Implementation()
{
	IEquippable::UnEquip_Implementation();
}

void AWeaponBase::ShootingCoolDown()
{
	bCanFire = true;
	ParticleSystemComponent->Deactivate();
}

float AWeaponBase::GetRange() const
{
	return Range;
}

float AWeaponBase::GetFireRate() const
{
	return FireRate;
}

float AWeaponBase::GetReLoadTime() const
{
	return ReLoadTime;
}

bool AWeaponBase::IsIsAutomatic() const
{
	return bIsAutomatic;
}

EWeaponType AWeaponBase::GetWeaponType() const
{
	return WeaponType;
}

ANewHaloCharacter* AWeaponBase::GetOwnerPlayer() const
{
	return OwnerPlayer;
}

FName AWeaponBase::GetAttachedSocketName() const
{
	return AttachedSocketName;
}

void AWeaponBase::SetAttachedSocketName(FName InAttachedSocketName)
{
	this->AttachedSocketName = InAttachedSocketName;
}

