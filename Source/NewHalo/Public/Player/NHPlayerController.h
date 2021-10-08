// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameUI/NewHaloHUD.h"
#include "NHPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NEWHALO_API ANHPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;
	
private:	

public:
	UFUNCTION(Client, Reliable)
	void SetStartGameTimer(float Time);

	UFUNCTION(Client, Reliable)
	void StartGame();

	UFUNCTION(Client, Reliable)
	void EndGame(float Time, ENHTeams WinningTeam);

protected:	

	UPROPERTY()
	ANewHaloHUD* HUD;
	
private:
	
};
