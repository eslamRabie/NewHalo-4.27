// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerSphere.h"
#include "Engine/TriggerVolume.h"
#include "ControlPoint.generated.h"

class ANewHaloCharacter;
class ANHPlayerState;
enum class ETeams;
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
	
	void OnOverLapBegin(AActor* ThisActor, AActor* OtherActor);
	
	void OnOverLapEnd(AActor* ThisActor, AActor* OtherActor);

private:
	int32 BlueTeamCount;
	int32 RedTeamCount;
	float Rate;
	ETeams ControlTeam;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Replicated)
	UStaticMeshComponent* ControlPointMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=ControlPoint)
	float RateFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=ControlPoint)
	float Radius;

	UPROPERTY()
	TArray<ANewHaloCharacter*> ConnectedPlayersList;
	
public:
	float GetRate() const;
	ETeams GetControlTeam() const;

protected:
	void UpdateRate();
};
