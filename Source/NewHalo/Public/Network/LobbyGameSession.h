// Fill out your copyright notice in the Description page of Project OnSettings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "LobbyGameSession.generated.h"

UCLASS()
class NEWHALO_API ALobbyGameSession : public AGameSession
{
	GENERATED_BODY()

public:
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
};
