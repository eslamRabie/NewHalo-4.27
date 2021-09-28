// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/NewHaloGameMode.h"
#include "GameUI/NewHaloHUD.h"
#include "Player/NewHaloCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANewHaloGameMode::ANewHaloGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ANewHaloHUD::StaticClass();
}

void ANewHaloGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ANewHaloGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ANewHaloGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
}

void ANewHaloGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}
