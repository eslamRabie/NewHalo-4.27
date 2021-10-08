// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NewHaloGameMode.h"
#include "GameFramework/GameMode.h"
#include "NHControlGameMode.generated.h"

class AControlPoint;
/**
 * 
 */
UCLASS()
class NEWHALO_API ANHControlGameMode : public ANewHaloGameMode
{
	GENERATED_BODY()

public:
	ANHControlGameMode();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

private:
	void GetControlPoint();
	void UpdateControlPointStats();
	
private:
	UPROPERTY()
	AControlPoint* GameControlPoint;

	FTimerHandle ControlPointTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Control)
	float ControlPointUpdateRateInSeconds;
};
