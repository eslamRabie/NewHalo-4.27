// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSystem/WeaponMagazineBase.h"
#include "WeaponSystem/WeaponProjectileBase.h"

// Sets default values
AWeaponMagazineBase::AWeaponMagazineBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootSceneComponent"));
	
	MagazineSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MagMesh"));
	MagazineSkeletalMeshComponent->SetupAttachment(RootComponent);
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

void AWeaponMagazineBase::Reload(int32 InAmount, int32& OutAmount)
{
	OutAmount = InAmount - (MaxAmmo - CurrentAmmoCount);
}

bool AWeaponMagazineBase::IsEmpty()
{
	return CurrentAmmoCount == 0;
}

AWeaponProjectileBase* AWeaponMagazineBase::GetProjectile()
{
	if(CurrentAmmoCount > 0)
	{
		CurrentAmmoCount--;
		return GetWorld()->SpawnActor<AWeaponProjectileBase>(GetActorLocation(), GetActorRotation());
	}
	else
	{
		// Play Empty Mag Sound
		return nullptr;
	}
}

