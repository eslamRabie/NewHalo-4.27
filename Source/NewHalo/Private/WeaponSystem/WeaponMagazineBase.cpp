// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSystem/WeaponMagazineBase.h"
#include "WeaponSystem/WeaponProjectileBase.h"

// Sets default values
AWeaponMagazineBase::AWeaponMagazineBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MagazineMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagMesh"));
	MagazineMeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponMagazineBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmoCount = MaxAmmo;
}

// Called every frame
void AWeaponMagazineBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponMagazineBase::Reload()
{
	auto ReloadAmount = FMath::Min(MaxAmmo - CurrentAmmoCount, AmmoPack); 
	AmmoPack -= ReloadAmount;
	CurrentAmmoCount += ReloadAmount;
}

bool AWeaponMagazineBase::IsEmpty()
{
	return CurrentAmmoCount == 0;
}

AWeaponProjectileBase* AWeaponMagazineBase::GetProjectile(FVector Location, FRotator Rotation)
{
	if(CurrentAmmoCount > 0)
	{
		CurrentAmmoCount--;
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		return GetWorld()->SpawnActor<AWeaponProjectileBase>(ProjectileClass, Location, Rotation, ActorSpawnParams);
	}
	else
	{
		// Play Empty Mag Sound
		return nullptr;
	}
}

int32 AWeaponMagazineBase::GetMaxAmmo() const
{
	return MaxAmmo;
}

int32 AWeaponMagazineBase::GetCurrentAmmoCount() const
{
	return CurrentAmmoCount;
}

int32 AWeaponMagazineBase::GetAmmoPack() const
{
	return AmmoPack;
}

