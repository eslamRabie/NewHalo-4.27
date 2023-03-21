// Fill out your copyright notice in the Description page of Project Settings.


#include "Control/ControlPoint.h"

#include "Player/NewHaloCharacter.h"
#include "Player/NHPlayerState.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Game/NewHaloGameMode.h"
#include "Game/NHControlGameMode.h"
#include "Kismet/GameplayStatics.h"

AControlPoint::AControlPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	BlueTeamCount = 0;
	RedTeamCount = 0;
	RateFactor = 0.1;
	MaxTime = 5 * 60;
	bIsControlled = false;
	ControlTeam = ENHTeams::None;

	ControlPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControlPointMesh"));
	ControlPointMesh->SetupAttachment(RootComponent);
	bIsGameOver = false;
}

void AControlPoint::BeginPlay()
{
	Super::BeginPlay();
	auto NHCintrolGameMode = GetWorld()->GetAuthGameMode<ANHControlGameMode>();
	if (!NHCintrolGameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant Get Game Mode in %s"), *GetName())
	}
	else
	{
		OnWinDelegate.BindUObject(NHCintrolGameMode, &ANewHaloGameMode::SetWinner);
	}
	SetActorHiddenInGame(false);
	RootComponent->SetHiddenInGame(true, true);
	ControlPointMesh->SetHiddenInGame(false, true);
	OnActorBeginOverlap.AddDynamic(this, &AControlPoint::OnOverLapBegin);
	OnActorEndOverlap.AddDynamic(this, &AControlPoint::OnOverLapEnd);
	CurrentTime = MaxTime;
}


void AControlPoint::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FColor Color;
	for (auto Player : ConnectedPlayersList)
	{
		if(!Player) continue;
		
		auto PS = Player->GetPlayerState<ANHPlayerState>();
		if (PS)
		{
			Color = PS->GetPlayerTeam() == ENHTeams::BlueTeam ? Color = FColor::Blue : Color = FColor::Red;
			DrawDebugLine(GetWorld(), GetActorLocation(), Player->GetActorLocation(),
			              Color, false, -1, 0, 2);
		}
	}
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		if (bIsControlled)
		{
			CurrentTime -= (DeltaSeconds * Rate);
			if (CurrentTime <= 0 && !bIsGameOver)
			{
				bIsGameOver = OnWinDelegate.ExecuteIfBound(ControlTeam);
				
			}
		}
		else
		{
			CurrentTime = MaxTime;
		}
	}
}

void AControlPoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// This actually takes care of replicating the Variable
	DOREPLIFETIME(AControlPoint, CurrentTime);
	DOREPLIFETIME(AControlPoint, ControlTeam);
}

void AControlPoint::OnOverLapEnd(AActor* ThisActor, AActor* OtherActor)
{
	if(!OtherActor) return;
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		auto Player = Cast<ANewHaloCharacter>(OtherActor);
		if (Player)
		{
			auto PS = Player->GetPlayerState<ANHPlayerState>();
			if (PS)
			{
				ConnectedPlayersList.Remove(Player);
				if (PS->GetPlayerTeam() == ENHTeams::BlueTeam)
				{
					BlueTeamCount--;
				}
				if (PS->GetPlayerTeam() == ENHTeams::RedTeam)
				{
					RedTeamCount--;
				}
				UpdateRate();
			}
		}
	}
}

float AControlPoint::GetCurrentTime() const
{
	return CurrentTime;
}

float AControlPoint::GetMaxTime() const
{
	return MaxTime;
}

void AControlPoint::OnOverLapBegin(AActor* ThisActor, AActor* OtherActor)
{
	if(!OtherActor) return;
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		auto Player = Cast<ANewHaloCharacter>(OtherActor);
		if (Player)
		{
			auto PS = Player->GetPlayerState<ANHPlayerState>();
			if (PS)
			{
				ConnectedPlayersList.Add(Player);
				if (PS->GetPlayerTeam() == ENHTeams::BlueTeam)
				{
					BlueTeamCount++;
				}
				if (PS->GetPlayerTeam() == ENHTeams::RedTeam)
				{
					RedTeamCount++;
				}
				UpdateRate();
			}
		}
	}
}

float AControlPoint::GetRate() const
{
	return Rate;
}

ENHTeams AControlPoint::GetControlTeam() const
{
	return ControlTeam;
}

void AControlPoint::RegisterGameEndEvent(ANHControlGameState* GameState, FName CallbackName)
{
	if(!GameState) return;
	OnWinDelegate.BindUFunction(GameState, CallbackName);
}

void AControlPoint::UpdateRate()
{
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		if (RedTeamCount == BlueTeamCount)
		{
			ControlTeam = ENHTeams::None;
			bIsControlled = false;
		}
		else if (RedTeamCount > BlueTeamCount)
		{
			ControlTeam = ENHTeams::RedTeam;
			bIsControlled = true;
		}
		else
		{
			ControlTeam = ENHTeams::BlueTeam;
			bIsControlled = true;
		}
		Rate = 1 + (RateFactor * FMath::Abs(BlueTeamCount - RedTeamCount));
	}
}
