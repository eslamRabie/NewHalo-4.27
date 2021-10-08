// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NHControlGameMode.h"

#include "Control/ControlPoint.h"
#include "Control/ControlPointSpawner.h"
#include "Kismet/GameplayStatics.h"

ANHControlGameMode::ANHControlGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	ControlPointUpdateRateInSeconds = 1;
}

void ANHControlGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ANHControlGameMode::BeginPlay()
{
	Super::BeginPlay();
	GetControlPoint();
	if(!GameControlPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't SpawnControlPoint Game will End"));
		return;
	}
	GetWorldTimerManager().SetTimer(ControlPointTimerHandle, this, &ANHControlGameMode::UpdateControlPointStats,
		ControlPointUpdateRateInSeconds, true);
}

void ANHControlGameMode::GetControlPoint()
{
	TArray<AActor*> ActorsOfClass; 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AControlPointSpawner::StaticClass(), ActorsOfClass);

	if(ActorsOfClass.Num() <= 0)
	{
		return;
	}
	
	auto SpawnerActor = ActorsOfClass[FMath::RandRange(0, ActorsOfClass.Num() - 1)];
	if(!SpawnerActor)
	{
		return;
	}
	auto ControlSpawner = Cast<AControlPointSpawner>(SpawnerActor);

	if(!ControlSpawner)
	{
		return;
	}
	GameControlPoint = ControlSpawner->SpawnControlPoint();
}

void ANHControlGameMode::UpdateControlPointStats()
{
	auto Team = GameControlPoint->GetControlTeam();
}
