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
	Kills = 0;
	Deaths = 0;
	bIsWaitingKill = false;
}

void ANHPlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ANHPlayerState::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	
	GM = GetWorld()->GetAuthGameMode<ANewHaloGameMode>();
	
	
}

void ANHPlayerState::Reset()
{
	Super::Reset();
	Health = MaxHealth;
	bIsWaitingKill = false;
}

void ANHPlayerState::ReduceHealth_Implementation(ANHPlayerState* ShooterPS, float Amount)
{
	if(!ShooterPS)
	{
		UE_LOG(LogTemp, Error, TEXT("ShooterPS is null in: %s"), *GetName())
		return;
	}
	Health -= Amount;
	if (Health <= 0 && !bIsWaitingKill)
	{
		if (GM)
		{
			UE_LOG(LogTemp, Error, TEXT("PS Dead in: %s"), *GetName())
			Deaths++;
			bIsWaitingKill = true;
			GM->AddKills(ShooterPS, this);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GM is null in: %s"), *GetName())
		}
	}
}

void ANHPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// This actually takes care of replicating the Variable
	DOREPLIFETIME(ANHPlayerState, Health);
	DOREPLIFETIME(ANHPlayerState, Kills);
	DOREPLIFETIME(ANHPlayerState, Deaths);
	DOREPLIFETIME(ANHPlayerState, bIsWaitingKill);
	DOREPLIFETIME(ANHPlayerState, PlayerTeam);
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

float ANHPlayerState::GetHealthPercent()
{
	return Health/MaxHealth;
}

void ANHPlayerState::GetPC_Implementation()
{
	PC = Cast<ANHPlayerController>(GetNetOwningPlayer()->GetPlayerController(GetWorld()));
	if(!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("No PC Fuck you %s"), *GetName())
	}
}


void ANHPlayerState::AddKills_Implementation()
{
	Kills++;
}

void ANHPlayerState::NotifyKill_Implementation(ANHPlayerState* ShooterPS, ANHPlayerState* TargetPS)
{
	if(!HUD)
	{
		if(!PC)
		{
			UE_LOG(LogTemp, Error, TEXT("Try To Get PC in NHPlayerState::NotifyKill"))
			PC = GetWorld()->GetFirstPlayerController<ANHPlayerController>();
		}
		if(PC)
		{
			HUD = PC->GetHUD<ANewHaloHUD>();
		}
	}
	if(HUD)
	{
		if (this == ShooterPS)
		{
			HUD->NotifyKill("You", TargetPS->GetPlayerName());
		}
		else if (this == TargetPS)
		{
			HUD->NotifyKill(ShooterPS->GetPlayerName(), "You");
		}
		else
		{
			HUD->NotifyKill(ShooterPS->GetPlayerName(), TargetPS->GetPlayerName());
		}
	}
	
}




void ANHPlayerState::SetPlayerTeam(ENHTeams InTeam)
{
	PlayerTeam = InTeam;
}


ENHTeams ANHPlayerState::GetPlayerTeam() const
{
	return PlayerTeam;
}

