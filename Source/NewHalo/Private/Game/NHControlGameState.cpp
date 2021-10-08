// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NHControlGameState.h"

#include "Net/UnrealNetwork.h"

ENHTeams ANHControlGameState::GetControlTeam() const
{
	return ControlTeam;
}

void ANHControlGameState::SetControlTeam(ENHTeams InControlTeam)
{
	if(HasAuthority() && GetNetMode() < ENetMode::NM_Client)
	{
		this->ControlTeam = InControlTeam;
	}
}

void ANHControlGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANHControlGameState, ControlTeam);
}