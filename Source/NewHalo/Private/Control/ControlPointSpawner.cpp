// Fill out your copyright notice in the Description page of Project Settings.


#include "Control/ControlPointSpawner.h"

#include "Control/ControlPoint.h"

// Sets default values
AControlPointSpawner::AControlPointSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AControlPointSpawner::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AControlPointSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AControlPoint* AControlPointSpawner::SpawnControlPoint()
{
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		if (!ControlPointClass)
		{
			UE_LOG(LogTemp, Error, TEXT("Controlpoint class must be set in BP in: %s "), *GetName());
			return nullptr;
		}
		auto World = GetWorld();
		if (!World)
		{
			return nullptr;
		}
		FActorSpawnParameters SP;
		SP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ControlPoint = World->SpawnActor<AControlPoint>(ControlPointClass, GetActorLocation(), GetActorRotation(), SP);
		//ControlPoint->SetActorLocationAndRotation(GetActorLocation(), GetActorRotation());
		if (!ControlPoint)
		{
			return nullptr;
		}
		return ControlPoint;
	}
	return nullptr;
}

AControlPoint* AControlPointSpawner::GetControlPoint()
{
	return ControlPoint;
}
