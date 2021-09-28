// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NHGameStateBase.h"
#include "Player/NHPlayerState.h"
#include "NHControlGameState.generated.h"

/**
 * 
 */
UCLASS()
class NEWHALO_API ANHControlGameState : public ANHGameStateBase
{
	GENERATED_BODY()


private:

	UPROPERTY()
	ETeams ControlTeam;
	
};
