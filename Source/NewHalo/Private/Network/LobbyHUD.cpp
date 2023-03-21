// Fill out your copyright notice in the Description page of Project OnSettings.


#include "Network/LobbyHUD.h"

#include "Blueprint/UserWidget.h"

#include "Network/LobbyWidget.h"

void ALobbyHUD::AddGameMode(ENHGameModes GameModeType, FString Name)
{
	if (LobbyWidget)
	{
		LobbyWidget->AddGameMode(GameModeType, Name);
	}
	else
	{
	}
}

void ALobbyHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (LobbyWidgetClass)
	{
		LobbyWidget = CreateWidget<ULobbyWidget>(GetWorld(), LobbyWidgetClass);
		if (LobbyWidget)
		{
			LobbyWidget->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HUD PostInitializeComponents LobbyWidgetClass is null"));
	}
}
