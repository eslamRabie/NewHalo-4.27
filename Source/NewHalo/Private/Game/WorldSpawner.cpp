// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/WorldSpawner.h"

// Sets default values
AWorldSpawner::AWorldSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SpawnCountRange = FIntPoint(2, 5);
	SpawnTimeRange = FVector2D(120, 360);
}

// Called when the game starts or when spawned
void AWorldSpawner::BeginPlay()
{
	Super::BeginPlay();
	SpawnCount = FMath::RandRange(SpawnCountRange.X, SpawnCountRange.Y);
	Spawn();
}

void AWorldSpawner::Spawn()
{
	if (SpawnedActorsList.Num() == 0 || SpawnCount == 0)
	{
		return;
	}
	int32 RandomActorIndex = FMath::RandRange(0, SpawnedActorsList.Num() - 1);

	const FVector Loc = GetActorLocation();
	const FRotator Rot = GetActorRotation();
	FActorSpawnParameters Asp;
	Asp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	SpawnedActor = GetWorld()->SpawnActor<AActor>(SpawnedActorsList[RandomActorIndex], Loc, Rot, Asp);
	SpawnedActor->SetReplicates(true);
	SpawnedActor->SetReplicateMovement(true);
	float TimeToRespawn = FMath::RandRange(SpawnTimeRange.X, SpawnTimeRange.Y);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AWorldSpawner::Spawn, TimeToRespawn);
}

