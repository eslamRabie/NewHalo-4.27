// Fill out your copyright notice in the Description page of Project OnSettings.

#pragma once

#include "CoreMinimal.h"
#include "Network/LobbyPlayerState.h"
#include "NHPlayerState.generated.h"

/**
 * 
 */

class ANewHaloCharacter;
class ANewHaloHUD;
class ANewHaloGameMode;
class ANHPlayerController;

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
	UFUNCTION(Server, Reliable)
	void ReduceHealth(ANHPlayerState* ShooterPS, float Amount);

	virtual void Reset() override;


	UPROPERTY()
	ANHPlayerController* PC;

	UPROPERTY()
	ANewHaloGameMode* GM;

	UPROPERTY()
	ANewHaloHUD* HUD;

	UPROPERTY()
	FString PlayerSessionId;

	UPROPERTY()
	FString MatchmakingPlayerId;

	UPROPERTY()
	FString Team;

	UFUNCTION(BlueprintCallable)
	void SetPlayerTeam(ENHTeams InTeam);

	UFUNCTION(BlueprintCallable)
	ENHTeams GetPlayerTeam() const;
	
	
protected:
	UPROPERTY(Replicated)
	ENHTeams PlayerTeam;
private:
	UPROPERTY(Replicated)
	float Health;

	UPROPERTY()
	float MaxHealth;

	UPROPERTY(Replicated)
	float Kills;

	UPROPERTY(Replicated)
	float Deaths;

	UPROPERTY(Replicated)
	bool bIsWaitingKill;

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
	UFUNCTION()
	float GetHealthPercent();
	UFUNCTION(Server, Reliable)
	void GetPC();


	UFUNCTION(BlueprintCallable, Client, Reliable)
	void NotifyKill(ANHPlayerState* ShooterPS, ANHPlayerState* TargetPS);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void AddKills();
	
};
