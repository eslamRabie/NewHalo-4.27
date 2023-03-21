// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUI/PlayerOverheadStatus.h"

#include "Components/ProgressBar.h"
#include "Player/NHPlayerState.h"

void UPlayerOverheadStatus::UpdateHealth(float Percent)
{
	if(!Health) return;
	Health->Percent = Percent;
}

void UPlayerOverheadStatus::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	PlayerTeam = ENHTeams::None;
}

void UPlayerOverheadStatus::NativeTick(const FGeometry& MovieSceneBlends, float InDeltaTime)
{
	Super::NativeTick(MovieSceneBlends, InDeltaTime);
}

void UPlayerOverheadStatus::NativeConstruct()
{
	Super::NativeConstruct();
}

