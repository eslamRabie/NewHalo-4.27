// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "WeaponProjectileBase.generated.h"

class ANewHaloCharacter;
UCLASS()
class NEWHALO_API AWeaponProjectileBase : public AActor
{
	GENERATED_BODY()
	
	public:	
	// Sets default values for this actor's properties
	AWeaponProjectileBase();

	protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnShoot(ANewHaloCharacter* OwnerCharacter, FVector InDirection,float InRange, float InDamage);

	void OnHit();
	
	
	private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=_Projectile)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=_Projectile)
	UProjectileMovementComponent* MovementComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"), Category=_Projectile)
	float SpeedCMPerSec;

	float DamageFactor;

	float Range;

	UPROPERTY()
	ANewHaloCharacter* HitActor;

	UPROPERTY()
	ANewHaloCharacter* OwnerCharacter;
	
	FTimerHandle TimerHandle;
	FHitResult Hit;
	
};
