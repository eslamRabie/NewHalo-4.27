// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NHPlayerState.generated.h"

/**
 * 
 */

class ANewHaloCharacter;
class ANewHaloHUD;
class ANewHaloGameMode;
class ANHPlayerController;
UENUM(BlueprintType, Blueprintable)
enum class ENHTeams: uint8
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

	void ReduceHealth(ANHPlayerController* ShooterPC, float Amount);
	
	UFUNCTION()
	void OnRep_Health();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetPlayerTeam(ENHTeams Team);
	

	UPROPERTY()
	ANHPlayerController* PC;

	UPROPERTY()
	ANewHaloGameMode* GM;

	UPROPERTY()
	ANewHaloHUD* HUD;

private:
	void TryGetHUD();
	
protected:	


private:
	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float Health;

	UPROPERTY()
	float MaxHealth;
	
	UPROPERTY(Replicated)
	float Kills;

	UPROPERTY(Replicated)
	float Deaths;

	UPROPERTY(Replicated)
	ENHTeams PlayerTeam;

	FTimerHandle TimerHandle;
	
public:
	UFUNCTION(BlueprintCallable)
	float GetHealth() const;
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable)
	float GetKills() const;
	UFUNCTION(BlueprintCallable)
	float GetDeaths() const;
	UFUNCTION(BlueprintCallable)
	ENHTeams GetPlayerTeam() const;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NotifyKill(ANHPlayerState* ShooterPS, ANHPlayerState* TargetPS);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void AddKills();
	
	void RegisterLocalCharacter(ANewHaloCharacter* PlayerCharacter);

	TDelegate<void(float)> HealthUpdate;
	
};


