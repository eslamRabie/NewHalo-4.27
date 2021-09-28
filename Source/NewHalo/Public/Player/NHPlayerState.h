// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NHPlayerState.generated.h"

/**
 * 
 */

UENUM()
enum class ETeams
{
	None,
	BlueTeam,
	RedTeam
};
UCLASS()
class NEWHALO_API ANHPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	ANHPlayerState();

	virtual void Tick(float DeltaSeconds) override;
	
protected:	

	virtual void BeginPlay() override;

	
	
public:

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_Kills();

	UFUNCTION()
	void OnRep_Deaths();

	UFUNCTION()
	void SetPlayerTeam(ETeams Team);

	UFUNCTION(Client, Reliable)
	void ClientSetPlayerTeam(ETeams Team);
	
	virtual void OnRep_Score() override;

	
protected:	


private:
	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float Health;

	UPROPERTY()
	float MaxHealth;
	
	UPROPERTY(ReplicatedUsing=OnRep_Kills)
	float Kills;

	UPROPERTY(ReplicatedUsing=OnRep_Deaths)
	float Deaths;

	UPROPERTY()
	ETeams PlayerTeam;
public:
	float GetHealth() const;
	float GetMaxHealth() const;
	float GetKills() const;
	float GetDeaths() const;
	ETeams GetPlayerTeam() const;
};


