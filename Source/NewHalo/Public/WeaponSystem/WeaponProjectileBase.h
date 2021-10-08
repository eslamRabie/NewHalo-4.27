// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "WeaponProjectileBase.generated.h"

class USphereComponent;
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

	void OnShoot(ANewHaloCharacter* OwnerCharacter, float InRange, float InDamage);

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=_Projectile)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=_Projectile)
	UProjectileMovementComponent* ProjectileMovement;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"), Category=_Projectile)
	float SpeedCMPerSec;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"), Category=_Projectile)
	FVector MovementOffset;
	

	float DamageFactor;

	float Range;


	UPROPERTY()
	ANewHaloCharacter* OwnerCharacter;
	


	
};
