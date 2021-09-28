// Fill out your copyright notice in the Description page of Project Settings.


#include "Control/ControlPoint.h"

#include "Player/NewHaloCharacter.h"
#include "Player/NHPlayerState.h"
#include "DrawDebugHelpers.h"
#include "Components/BrushComponent.h"

AControlPoint::AControlPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	BlueTeamCount = 0;
	RedTeamCount = 0;
	RateFactor = 0.5;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	
	ControlPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControlPointMesh"));
	ControlPointMesh->SetupAttachment(RootComponent);
	
	
	OnActorBeginOverlap.AddDynamic(this, &AControlPoint::OnOverLapBegin);
	OnActorEndOverlap.AddDynamic(this, &AControlPoint::OnOverLapEnd);
	
}

void AControlPoint::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	for (auto Player : ConnectedPlayersList)
	{
		auto PS = Player->GetPlayerState<ANHPlayerState>();
		if(PS)
		{
			FColor Color = PS->GetPlayerTeam() == ETeams::BlueTeam? FColor::Blue : FColor::Red;
			DrawDebugLine(GetWorld(), ControlPointMesh->GetComponentLocation(), Player->GetActorLocation(),
				Color, false, -1, 0, 2);
		}
	}
	
}

void AControlPoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// This actually takes care of replicating the Variable
	DOREPLIFETIME(AControlPoint, ControlPointMesh);
}

void AControlPoint::OnOverLapEnd(AActor* ThisActor, AActor* OtherActor)
{
	auto Player = Cast<ANewHaloCharacter>(OtherActor);
	if(Player)
	{
		auto PS = Player->GetPlayerState<ANHPlayerState>();
		if(PS)
		{
			ConnectedPlayersList.Remove(Player);
			if(PS->GetPlayerTeam() == ETeams::BlueTeam)
			{
				BlueTeamCount--;
			}
			if(PS->GetPlayerTeam() == ETeams::RedTeam)
			{
				RedTeamCount--;
			}
			UpdateRate();
		}
	}
}

void AControlPoint::OnOverLapBegin(AActor* ThisActor, AActor* OtherActor)
{
	auto Player = Cast<ANewHaloCharacter>(OtherActor);
	if(Player)
	{
		auto PS = Player->GetPlayerState<ANHPlayerState>();
		if(PS)
		{
			ConnectedPlayersList.Add(Player);
			if(PS->GetPlayerTeam() == ETeams::BlueTeam)
			{
				BlueTeamCount++;
			}
			if(PS->GetPlayerTeam() == ETeams::RedTeam)
			{
				RedTeamCount++;
			}
			UpdateRate();
		}
	}
}

float AControlPoint::GetRate() const
{
	return Rate;
}

ETeams AControlPoint::GetControlTeam() const
{
	return ControlTeam;
}

void AControlPoint::UpdateRate()
{
	Rate = RateFactor * FMath::Abs(BlueTeamCount - RedTeamCount);
}
