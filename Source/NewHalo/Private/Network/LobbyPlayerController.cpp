// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/LobbyPlayerController.h"

#include "Network/LobbyGameMode.h"
#include "Network/LobbyHUD.h"

ALobbyPlayerController::ALobbyPlayerController()
{
}

void ALobbyPlayerController::Client_PopulateGameModesHUD_Implementation(ENHGameModes GameModeType,
                                                                        const FString& ModeName)
{
	auto Hud = GetHUD<ALobbyHUD>();
	if (Hud)
	{
		Hud->AddGameMode(GameModeType, ModeName);
	}
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeUIOnly InputModeUI;
	InputModeUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputModeUI);
	bShowMouseCursor = true;
}
