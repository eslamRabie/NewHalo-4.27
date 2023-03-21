// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NHControlGameState.h"

#include "Control/ControlPoint.h"
#include "Control/ControlPointSpawner.h"
#include "Game/NewHaloGameMode.h"
#include "Game/NHControlGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/NHPlayerController.h"

ANHControlGameState::ANHControlGameState()
{
	bReplicates = true;
	ControlPointUpdateRateInSeconds = 1;
}

ENHTeams ANHControlGameState::GetControlTeam() const
{
	return ControlTeam;
}

void ANHControlGameState::SetControlTeam(ENHTeams InControlTeam)
{
	if (AuthorityGameMode)
	{
		this->ControlTeam = InControlTeam;
	}
}

void ANHControlGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

// void ANHControlGameState::NotifyStartGame_Implementation(float Time)
// {
// 	auto PC = GetWorld()->GetFirstPlayerController<ANHPlayerController>();
// 	if(PC)
// 		PC->SetStartGameTimer(Time);
// }
//
// void ANHControlGameState::NotifyStartGameEnding_Implementation()
// {
// 	auto PC = GetWorld()->GetFirstPlayerController<ANHPlayerController>();
// 	if(PC)
// 		PC->StartGame();
// }

void ANHControlGameState::NotifyGameEnding_Implementation(float Time)
{
	auto PC = GetWorld()->GetFirstPlayerController<ANHPlayerController>();
	if(PC)
		PC->EndGame(Time, GetWinningTeam());
}

void ANHControlGameState::NotifyWin_Implementation(ENHTeams InWiningTeam)
{
	SetWinningTeam(InWiningTeam);
	if(AuthorityGameMode)
	{
		auto GM = Cast<ANHControlGameMode>(AuthorityGameMode);
		if(GM)
		{
			GM->SetWinner(InWiningTeam);
		}
	}
}

void ANHControlGameState::BeginPlay()
{
	Super::BeginPlay();
	if(AuthorityGameMode)
	{
		GetWorldTimerManager().SetTimer(ControlPointTimerHandle, this, &ANHControlGameState::UpdateControlPointStats,
									ControlPointUpdateRateInSeconds, true, 60);
	}
}

void ANHControlGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANHControlGameState, ControlTeam);
	DOREPLIFETIME(ANHControlGameState, GameControlPoint);
}


void ANHControlGameState::GetControlPoint()
{
	TArray<AActor*> ActorsOfClass;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AControlPointSpawner::StaticClass(), ActorsOfClass);

	if (ActorsOfClass.Num() <= 0)
	{
		return;
	}

	auto SpawnerActor = ActorsOfClass[FMath::RandRange(0, ActorsOfClass.Num() - 1)];
	if (!SpawnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't get SpawnerActor "));
		return;
	}
	auto ControlSpawner = Cast<AControlPointSpawner>(SpawnerActor);

	if (!ControlSpawner)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't get ControlSpawner "));
		return;
	}
	GameControlPoint = ControlSpawner->SpawnControlPoint();
	if(GameControlPoint)
	{
		GameControlPoint->RegisterGameEndEvent(this, FName("NotifyWin"));
	}
}

void ANHControlGameState::UpdateControlPointStats_Implementation()
{
	if (!GameControlPoint)
	{
		GetControlPoint();
		UE_LOG(LogTemp, Error, TEXT("Try to get SpawnControlPoint"));
		return;
	}
	if(GameControlPoint)
	{
		ControlTeam = GameControlPoint->GetControlTeam();
	}
}

float ANHControlGameState::GetControlPointTime()
{
	if(GameControlPoint)
		return GameControlPoint->GetCurrentTime();
	else
		return -1;
}
