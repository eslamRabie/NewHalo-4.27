// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerSphere.h"
#include "Engine/TriggerVolume.h"
#include "Player/NHPlayerState.h"
#include "ControlPoint.generated.h"

class ANewHaloCharacter;
class ANHPlayerState;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class NEWHALO_API AControlPoint : public ATriggerSphere
{
	GENERATED_BODY()

public:

	AControlPoint();

	virtual void Tick(float DeltaSeconds) override;

protected:

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnOverLapBegin(AActor* ThisActor, AActor* OtherActor);
	UFUNCTION()
	void OnOverLapEnd(AActor* ThisActor, AActor* OtherActor);


	
public:
	float GetCurrentTime() const;
	float GetMaxTime() const;
private:
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Replicated)
	UStaticMeshComponent* ControlPointMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=ControlPoint)
	float RateFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=ControlPoint)
	float Radius;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=ControlPoint)
	float MaxTime;

	UPROPERTY(Replicated)
	float CurrentTime;

	UPROPERTY(Replicated)
	ENHTeams ControlTeam;

	
	int32 BlueTeamCount;
	int32 RedTeamCount;
	float Rate;
	
	UPROPERTY()
	bool bIsControlled;
	
	UPROPERTY()
	TArray<ANewHaloCharacter*> ConnectedPlayersList;

	TDelegate<void(ENHTeams)> OnWinDelegate;
	
public:
	float GetRate() const;
	ENHTeams GetControlTeam() const;

protected:
	void UpdateRate();
};
