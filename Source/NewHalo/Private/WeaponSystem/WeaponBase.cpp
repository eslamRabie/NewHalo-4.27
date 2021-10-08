// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSystem/WeaponBase.h"

#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/NewHaloCharacter.h"
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

	AttachedSocketName = TEXT("None");

	bStopFiring = true;
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

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBase::Fire(FVector Location, FRotator Rotation)
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
		if(EmptyMagazineSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EmptyMagazineSound, GetActorLocation());
		}
		return;
	}

	
	auto Projectile = WeaponMagazine->GetProjectile(WeaponSkeletalMeshComponent->GetSocketLocation(MuzzleSocketName),
		WeaponSkeletalMeshComponent->GetSocketRotation(MuzzleSocketName));
	
	if(Projectile)
	{
		bCanFire = false;
		ParticleSystemComponent->Activate();
		Projectile->OnShoot(OwnerPlayer, Range, WeaponDamage);
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeaponBase::ShootingCoolDown, FireRate, false);

		// try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = OwnerPlayer->GetMesh()->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}
	}
}

void AWeaponBase::ShootingCoolDown()
{
	bCanFire = true;
	ParticleSystemComponent->Deactivate();
	if(bIsAutomatic && !bStopFiring)
	{
		Fire(OwnerPlayer->GetMuzzleLocation(), OwnerPlayer->GetActorRotation());
	}
}


void AWeaponBase::Reload()
{
	WeaponMagazine->Reload();

	if(ReloadAnimation)
	{
		UAnimInstance* AnimInstance = OwnerPlayer->GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(ReloadAnimation, 1.f);
		}
	}
	if(ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());
	}
}

FVector AWeaponBase::GetAmmo()
{
	return FVector(WeaponMagazine->GetCurrentAmmoCount(), WeaponMagazine->GetMaxAmmo(), WeaponMagazine->GetAmmoPack());
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
	UnEquip_Implementation();
}

void AWeaponBase::Equip_Implementation(FName SocketName)
{
	IEquippable::Equip_Implementation(SocketName);
	if(AttachedSocketName == TEXT("None"))
	{
		AttachedSocketName = SocketName;
	}
	AttachToComponent(OwnerPlayer->GetMesh3P(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
}

void AWeaponBase::UnEquip_Implementation()
{
	IEquippable::UnEquip_Implementation();
	AttachedSocketName = TEXT("None");
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
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

UTexture2D* AWeaponBase::GetWeaponIcon() const
{
	return WeaponIcon;
}

FName AWeaponBase::GetAttachedSocketName() const
{
	return AttachedSocketName;
}

void AWeaponBase::SetAttachedSocketName(FName InAttachedSocketName)
{
	this->AttachedSocketName = InAttachedSocketName;
}

