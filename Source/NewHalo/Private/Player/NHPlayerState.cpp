// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NHPlayerState.h"

#include "Game/NewHaloGameMode.h"
#include "GameUI/NewHaloHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/NewHaloCharacter.h"
#include "Player/NHPlayerController.h"

ANHPlayerState::ANHPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
	MaxHealth = 100;
}

void ANHPlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Health -= 0.5;
}

void ANHPlayerState::BeginPlay()
{
	Super::BeginPlay();
	if(HasLocalNetOwner())
	{
		PC = GetWorld()->GetFirstPlayerController<ANHPlayerController>();
		HUD = PC->GetHUD<ANewHaloHUD>();
		if(!HUD)
		{
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ANHPlayerState::TryGetHUD, 1);
		}
	}
	if(HasAuthority() && GetNetMode() < ENetMode::NM_Client)
	{
		Health = MaxHealth;
		Kills = 0;
		Deaths = 0;
		GM = GetWorld()->GetAuthGameMode<ANewHaloGameMode>();
	}
}

void ANHPlayerState::ReduceHealth(ANHPlayerController* ShooterPC, float Amount)
{
	if(!HasAuthority() || GetNetMode() == ENetMode::NM_Client)
	{
		return;
	}
	Health -= Amount;
	if(Health <= 0)
	{
		if(GM)
		{
			GM->AddKills(ShooterPC, PC);
		}
	}
}

void ANHPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// This actually takes care of replicating the Variable
	DOREPLIFETIME(ANHPlayerState, Health);
	DOREPLIFETIME(ANHPlayerState, Kills);
	DOREPLIFETIME(ANHPlayerState, Deaths);
}

void ANHPlayerState::OnRep_Health()
{
	if(HasLocalNetOwner())
	{
		if(HUD)
		{
			HUD->UpdateHealth(Health/MaxHealth);
		}
	}
	HealthUpdate.Execute(Health/MaxHealth);
}

void ANHPlayerState::SetPlayerTeam_Implementation(ENHTeams Team)
{
	PlayerTeam = Team;
}

void ANHPlayerState::TryGetHUD()
{
	HUD = PC->GetHUD<ANewHaloHUD>();
	if(!HUD)
	{
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ANHPlayerState::TryGetHUD, 1);
	}
}

float ANHPlayerState::GetHealth() const
{
	return Health;
}

float ANHPlayerState::GetMaxHealth() const
{
	return MaxHealth;
}

float ANHPlayerState::GetKills() const
{
	return Kills;
}

float ANHPlayerState::GetDeaths() const
{
	return Deaths;
}

ENHTeams ANHPlayerState::GetPlayerTeam() const
{
	return PlayerTeam;
}

void ANHPlayerState::RegisterLocalCharacter(ANewHaloCharacter* PlayerCharacter)
{
	HealthUpdate.BindUObject(PlayerCharacter, &ANewHaloCharacter::UpdateHealth);
}


void ANHPlayerState::AddKills_Implementation()
{
	if(HasAuthority() && GetNetMode() < ENetMode::NM_Client)
	{
		Kills++;
	}
}

void ANHPlayerState::NotifyKill_Implementation(ANHPlayerState* ShooterPS, ANHPlayerState* TargetPS)
{
	if(HasLocalNetOwner())
	{
		if(this == ShooterPS)
		{
			if(HUD)
			{
				HUD->NotifyKill("You", TargetPS->GetPlayerName());
			}
		}
		else if(this == TargetPS)
		{
			if(HUD)
			{
				HUD->NotifyKill(ShooterPS->GetPlayerName(), "You");
			}
		}
		else
		{
			if(HUD)
			{
				HUD->NotifyKill(ShooterPS->GetPlayerName(), TargetPS->GetPlayerName());
			}
		}
	}
}
