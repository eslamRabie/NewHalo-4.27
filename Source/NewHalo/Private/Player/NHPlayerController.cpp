// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NHPlayerController.h"

void ANHPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ANHPlayerController::BeginPlay()
{
	Super::BeginPlay();
	HUD = GetHUD<ANewHaloHUD>();
}

void ANHPlayerController::EndGame_Implementation(float Time, ENHTeams WinningTeam)
{
	DisableInput(this);
	if(HUD)
	{
		HUD->SetEndGameTimer(Time);
		HUD->ShowWinner(WinningTeam);
	}
}

void ANHPlayerController::StartGame_Implementation()
{
	EnableInput(this);
}

void ANHPlayerController::SetStartGameTimer_Implementation(float Time)
{
	DisableInput(this);
	if(HUD)
	{
		HUD->SetStartGameTimer(Time);
	}
}
