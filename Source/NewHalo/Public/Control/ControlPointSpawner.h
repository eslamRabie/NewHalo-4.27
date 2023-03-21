// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ControlPointSpawner.generated.h"

class AControlPoint;
UCLASS()
class NEWHALO_API AControlPointSpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AControlPointSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	AControlPoint* SpawnControlPoint();

	UFUNCTION(BlueprintCallable)
	AControlPoint* GetControlPoint();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=ControlPoint)
	TSubclassOf<AControlPoint> ControlPointClass;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=ControlPoint)
	AControlPoint* ControlPoint;
};
