// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Player/NHPlayerState.h"

#include "NewHaloGameMode.generated.h"

class UNHGameInstance;
class ANHPlayerController;
UCLASS(minimalapi)
class ANewHaloGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ANewHaloGameMode();

	virtual void Tick(float DeltaSeconds) override;
	

protected:
	virtual void BeginPlay() override;

	virtual void RestartPlayer(AController* NewPlayer) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	// Game Mode Interface
	virtual void HandleMatchIsWaitingToStart() override;

	virtual bool ReadyToStartMatch_Implementation() override;

	virtual void HandleMatchHasStarted() override;

	virtual bool ReadyToEndMatch_Implementation() override;

	
	virtual void HandleMatchHasEnded() override;
	
	virtual void HandleLeavingMap() override;
	
	virtual void HandleMatchAborted() override;
	
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

	virtual void StartMatch() override;

	virtual void EndMatch() override;


public:
/// Basic Player Functionality
/// 

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void ApplyDamageToPlayer(ANHPlayerController* ShooterPC, ANHPlayerController* TargetPC, float Damage);

	UFUNCTION(BlueprintCallable)
	virtual void SetWinner(ENHTeams WinningTeam);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void AddKills(ANHPlayerController* ShooterPC, ANHPlayerController* TargetPC);

private:
	UPROPERTY()
	TArray<ANHPlayerController*> PlayerControllers;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta=(AllowPrivateAccess), Category=GameDesign)
	float MatchStartWaitingTimeInS;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta=(AllowPrivateAccess), Category=GameDesign)
	float MatchEndWaitingTimeInS;

	FTimerHandle StartEndTimerHandle;

	bool bCanStartMatch;

	bool bCanEndMatch;
	
	
};



