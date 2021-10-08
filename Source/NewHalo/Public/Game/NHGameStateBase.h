// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Player/NHPlayerState.h"
#include "NHGameStateBase.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class NEWHALO_API ANHGameStateBase : public AGameState
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	ENHTeams WinningTeam;
	
public:
	ENHTeams GetWinningTeam() const;
	void SetWinningTeam(ENHTeams InWinningTeam);
	
};
