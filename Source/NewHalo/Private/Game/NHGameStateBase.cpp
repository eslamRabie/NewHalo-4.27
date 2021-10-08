// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NHGameStateBase.h"

#include "Net/UnrealNetwork.h"


ENHTeams ANHGameStateBase::GetWinningTeam() const
{
	return WinningTeam;
}

void ANHGameStateBase::SetWinningTeam(ENHTeams InWinningTeam)
{
	if(HasAuthority() && GetNetMode() < ENetMode::NM_Client)
	{
		this->WinningTeam = InWinningTeam;
	}
}

void ANHGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANHGameStateBase, WinningTeam);
}
