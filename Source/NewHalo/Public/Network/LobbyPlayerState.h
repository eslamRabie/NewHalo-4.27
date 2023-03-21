// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPlayerState.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class ENHTeams: uint8
{
	None,
	BlueTeam,
	RedTeam
};

UCLASS()
class NEWHALO_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALobbyPlayerState();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
