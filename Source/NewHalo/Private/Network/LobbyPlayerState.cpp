// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/LobbyPlayerState.h"

#include "Net/UnrealNetwork.h"


// Sets default values
ALobbyPlayerState::ALobbyPlayerState()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();
}
