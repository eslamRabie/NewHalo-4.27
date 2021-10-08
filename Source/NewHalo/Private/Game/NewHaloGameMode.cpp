// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/NewHaloGameMode.h"

#include "NHGameInstance.h"
#include "Game/NHGameStateBase.h"
#include "GameUI/NewHaloHUD.h"
#include "Player/NewHaloCharacter.h"
#include "Player/NHPlayerController.h"
#include "UObject/ConstructorHelpers.h"

ANewHaloGameMode::ANewHaloGameMode()
{
	// use our custom HUD class
	HUDClass = ANewHaloHUD::StaticClass();
	bCanStartMatch = false;
	bCanEndMatch = false;
	MatchStartWaitingTimeInS = 10;
	MatchEndWaitingTimeInS = 5;
}

void ANewHaloGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ANewHaloGameMode::BeginPlay()
{
	Super::BeginPlay();
	GSession = Cast<ANHGameSession>(GameSession);
}

void ANewHaloGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
	
}

void ANewHaloGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	
}

void ANewHaloGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
}

bool ANewHaloGameMode::ReadyToStartMatch_Implementation()
{

	// By default start when we have > 0 players
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		if (NumPlayers == GSession->MaxPlayers && !bCanStartMatch)
		{
			for (auto Player : PlayerControllers)
			{
				Player->DisableInput(Player);
				Player->SetStartGameTimer(MatchStartWaitingTimeInS);
			}
			GetWorldTimerManager().SetTimer(StartEndTimerHandle, this, &ANewHaloGameMode::StartMatch, MatchStartWaitingTimeInS);
		}
	}
	return false;
}



void ANewHaloGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

bool ANewHaloGameMode::ReadyToEndMatch_Implementation()
{
	return false;
}

void ANewHaloGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}

void ANewHaloGameMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();
}

void ANewHaloGameMode::HandleMatchAborted()
{
	Super::HandleMatchAborted();
}

void ANewHaloGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
}

void ANewHaloGameMode::StartMatch()
{
	Super::StartMatch();
	for (auto Player : PlayerControllers)
	{
		Player->EnableInput(Player);
		Player->StartGame();
	}
}

void ANewHaloGameMode::EndMatch()
{
	Super::EndMatch();
}

void ANewHaloGameMode::ApplyDamageToPlayer_Implementation(ANHPlayerController* ShooterPC ,ANHPlayerController* TargetPC, float Damage)
{
	if(HasAuthority() && GetNetMode() < ENetMode::NM_Client)
	{
		TargetPC->GetPlayerState<ANHPlayerState>()->ReduceHealth(ShooterPC, Damage);
	}
}

void ANewHaloGameMode::SetWinner(ENHTeams WinningTeam)
{
	for (auto Player : PlayerControllers)
	{
		Player->DisableInput(Player);
		Player->EndGame(MatchEndWaitingTimeInS, WinningTeam);
		auto GS = Cast<ANHGameStateBase>(GameState);
		GS->SetWinningTeam(WinningTeam);
	}
	GetWorldTimerManager().SetTimer(StartEndTimerHandle, this, &ANewHaloGameMode::EndMatch, MatchEndWaitingTimeInS);
}

void ANewHaloGameMode::AddKills_Implementation(ANHPlayerController* ShooterPC, ANHPlayerController* TargetPC)
{
	auto ShooterPS = ShooterPC->GetPlayerState<ANHPlayerState>();
	auto TargetPS = TargetPC->GetPlayerState<ANHPlayerState>();
	ShooterPS->AddKills();
	ShooterPS->NotifyKill(ShooterPS, TargetPS);
	RestartPlayer(TargetPC);
}
