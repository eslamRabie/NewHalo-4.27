// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSystem/WeaponProjectileBase.h"

#include "Components/SphereComponent.h"
#include "Player/NHPlayerState.h"
#include "Player/NewHaloCharacter.h"


// Sets default values
AWeaponProjectileBase::AWeaponProjectileBase()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AWeaponProjectileBase::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	MeshComponent->SetupAttachment(RootComponent);
	
	//
	bReplicates = true;

}

// Called when the game starts or when spawned
void AWeaponProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponProjectileBase::OnShoot(ANewHaloCharacter* InOwnerCharacter, float InRange, float InDamage)
{
	Range = InRange;
	DamageFactor = InDamage;
	OwnerCharacter = InOwnerCharacter;
}

void AWeaponProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor && OtherActor->IsA(ANewHaloCharacter::StaticClass()))
	{
		auto OtherCharacter = Cast<ANewHaloCharacter>(OtherActor);
		if(OtherCharacter && OtherCharacter != OwnerCharacter)
		{
			FPointDamageEvent PointDamageEvent(DamageFactor, Hit, GetActorLocation(), UDamageType::StaticClass());
			auto HitPS = OtherCharacter->GetPlayerState<ANHPlayerState>();
			auto OwnerPS = OwnerCharacter->GetPlayerState<ANHPlayerState>();
			if(HitPS && OwnerPS)
			{
				if(HitPS->GetPlayerTeam() != OwnerPS->GetPlayerTeam())
				{
					OtherActor->TakeDamage(DamageFactor, PointDamageEvent, OwnerCharacter->GetNetOwningPlayer()->GetPlayerController(GetWorld()), this);
				}
				else
				{
					OtherActor->TakeDamage(0, PointDamageEvent, OwnerCharacter->GetNetOwningPlayer()->GetPlayerController(GetWorld()), this);
				}
			}
		}
	}
	// Only add impulse and destroy projectile if we hit a physics
	else if (OtherActor && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}
	Destroy();
}

