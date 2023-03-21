// Fill out your copyright notice in the Description page of Project OnSettings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

enum class ENHGameModes;
/**
 * 
 */
UCLASS()
class NEWHALO_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ALobbyPlayerController();

	UFUNCTION(Client, Reliable)
	void Client_PopulateGameModesHUD(ENHGameModes GameModeType, const FString& ModeName);

protected:
	virtual void BeginPlay() override;
};
