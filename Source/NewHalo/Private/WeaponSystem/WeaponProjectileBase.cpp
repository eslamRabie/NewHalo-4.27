// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSystem/WeaponProjectileBase.h"

#include "Player/NHPlayerState.h"
#include "Player/NewHaloCharacter.h"


// Sets default values
AWeaponProjectileBase::AWeaponProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpeedCMPerSec = 10;

	RootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
	MovementComponent->Velocity = FVector::ZeroVector;

	MovementComponent->InitialSpeed = SpeedCMPerSec;
	MovementComponent->MaxSpeed = SpeedCMPerSec;
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->bShouldBounce = true;
	MovementComponent->Bounciness = 0.3f;
	MovementComponent->ProjectileGravityScale = 0.0f;

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

void AWeaponProjectileBase::OnShoot(ANewHaloCharacter* InOwnerCharacter, FVector Direction,float InRange, float InDamage)
{
	Range = InRange;
	DamageFactor = InDamage;
	OwnerCharacter = InOwnerCharacter;
	FCollisionQueryParams TraceParams;

	GetWorld()->LineTraceSingleByChannel(OUT Hit,GetActorLocation(),GetActorForwardVector() * Range,
		ECC_Visibility, TraceParams);

	auto Dist = GetDistanceTo(HitActor);
	auto Time = Dist / SpeedCMPerSec;


	//TODO Debug the projectile direction and movement 
	
	MovementComponent->Velocity = Direction * SpeedCMPerSec;
	UE_LOG(LogTemp, Error, TEXT("%f"), SpeedCMPerSec);
	UE_LOG(LogTemp, Error, TEXT("test %f, %s , %s"), SpeedCMPerSec, *GetActorLocation().ToString(), *MovementComponent->Velocity.ToString());

	if(Hit.GetActor()->IsA(ANewHaloCharacter::StaticClass()))
	{
		HitActor = Cast<ANewHaloCharacter>(Hit.GetActor());
		if(HitActor)
		{
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AWeaponProjectileBase::OnHit,Time, false);
		}
	}
	else
	{
		//TODO Check if the other actor can interact with projectiles  
	}
	
}

void AWeaponProjectileBase::OnHit()
{
	UE_LOG(LogTemp, Error, TEXT("Destroy"));
	FPointDamageEvent PointDamageEvent(DamageFactor, Hit, GetActorLocation(), UDamageType::StaticClass());
	auto HitPS = HitActor->GetPlayerState<ANHPlayerState>();
	auto OwnerPS = OwnerCharacter->GetPlayerState<ANHPlayerState>();
	if(HitPS && OwnerPS)
	{
		if(HitPS->GetPlayerTeam() != OwnerPS->GetPlayerTeam())
		{
			HitActor->TakeDamage(DamageFactor, PointDamageEvent, OwnerCharacter->GetNetOwningPlayer()->GetPlayerController(GetWorld()), this);
		}
		else
		{
			HitActor->TakeDamage(0, PointDamageEvent, OwnerCharacter->GetNetOwningPlayer()->GetPlayerController(GetWorld()), this);
		}
	}
	Destroy();
}

