// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldSpawner.generated.h"

UCLASS()
class NEWHALO_API AWorldSpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWorldSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Spawn();
	


private:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TArray<TSubclassOf<AActor>> SpawnedActorsList;

	UPROPERTY()
	AActor* SpawnedActor;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FVector2D SpawnTimeRange;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FIntPoint SpawnCountRange;


	int32 SpawnCount;

	FTimerHandle TimerHandle;
};
