// Fill out your copyright notice in the Description page of Project OnSettings.

#pragma once

#include "CoreMinimal.h"
#include "LobbyGameMode.h"
#include "GameFramework/HUD.h"
#include "LobbyHUD.generated.h"

class ULobbyWidget;
/**
 * 
 */
UCLASS()
class NEWHALO_API ALobbyHUD : public AHUD
{
	GENERATED_BODY()

public:
	void AddGameMode(ENHGameModes GameModeType, FString Name);

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

private:
	UPROPERTY(EditDefaultsOnly, Category=Lobby)
	TSubclassOf<ULobbyWidget> LobbyWidgetClass;

	UPROPERTY()
	ULobbyWidget* LobbyWidget;
};
