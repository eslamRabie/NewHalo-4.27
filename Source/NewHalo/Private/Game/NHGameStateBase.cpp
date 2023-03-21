// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NHGameStateBase.h"

#include "Net/UnrealNetwork.h"


ENHTeams ANHGameStateBase::GetWinningTeam() const
{
	return WinningTeam;
}

void ANHGameStateBase::SetWinningTeam_Implementation(ENHTeams InWinningTeam)
{
	WinningTeam = InWinningTeam;
}

void ANHGameStateBase::NotifyKill_Implementation(ANHPlayerState* ShooterPS, ANHPlayerState* TargetPS)
{
	for(auto Player: PlayerArray)
	{
		if(Player)
		{
			auto NHPS = Cast<ANHPlayerState>(Player);
			if(NHPS)
			{
				NHPS->NotifyKill(ShooterPS, TargetPS);
			}
		}
	}
}

void ANHGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANHGameStateBase, WinningTeam);
}
