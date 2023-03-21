// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NHGameStateBase.h"
#include "Player/NHPlayerState.h"
#include "NHControlGameState.generated.h"

class AControlPoint;
/**
 * 
 */
UCLASS()
class NEWHALO_API ANHControlGameState : public ANHGameStateBase
{
	GENERATED_BODY()


private:
	UPROPERTY(Replicated)
	ENHTeams ControlTeam;

public:
	ANHControlGameState();
	ENHTeams GetControlTeam() const;
	void SetControlTeam(ENHTeams InControlTeam);
	float GetControlPointTime();
	virtual void Tick(float DeltaSeconds) override;

	// UFUNCTION(NetMulticast, Reliable)
	// void NotifyStartGame(float Time);
	// UFUNCTION(NetMulticast, Reliable)
	// void NotifyStartGameEnding();
	UFUNCTION(NetMulticast, Reliable)
	void NotifyGameEnding(float Time);
	

	UFUNCTION(Server, Reliable)
	void NotifyWin(ENHTeams InWiningTeam);

protected:
	virtual void BeginPlay() override;

private:
	void GetControlPoint();
	
	UFUNCTION(Server, Reliable)
	void UpdateControlPointStats();


private:
	UPROPERTY(Replicated)
	AControlPoint* GameControlPoint;

	FTimerHandle ControlPointTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Control)
	float ControlPointUpdateRateInSeconds;
};
