// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NHPlayerState.h"

#include "GameUI/NewHaloHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ANHPlayerState::ANHPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
	MaxHealth = 100;
}

void ANHPlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Health -= 0.1;
}

void ANHPlayerState::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	Kills = 0;
	Deaths = 0;
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
	auto PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("Not PC in %s"), *GetName());
		return;
	}
	auto HUD = PC->GetHUD<ANewHaloHUD>();
	if(!HUD)
	{
		UE_LOG(LogTemp, Error, TEXT("Not HUD %s"), *GetName());
		return;
	}
	HUD->UpdateHealth(Health/MaxHealth);
}
void ANHPlayerState::OnRep_Kills()
{
}

void ANHPlayerState::OnRep_Deaths()
{
}

void ANHPlayerState::SetPlayerTeam(ETeams Team)
{
	PlayerTeam = Team;
	ClientSetPlayerTeam(Team);
}

void ANHPlayerState::ClientSetPlayerTeam_Implementation(ETeams Team)
{
	UE_LOG(LogTemp, Error, TEXT("Called On %s"), *GetName())
	PlayerTeam = Team;
}

void ANHPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
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

ETeams ANHPlayerState::GetPlayerTeam() const
{
	return PlayerTeam;
}
