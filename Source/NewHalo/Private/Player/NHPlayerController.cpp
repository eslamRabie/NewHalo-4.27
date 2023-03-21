// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NHPlayerController.h"

void ANHPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ANHPlayerController::BeginPlay()
{
	Super::BeginPlay();
	HUD = GetHUD<ANewHaloHUD>();
	
	if(InputEnabled())
		UE_LOG(LogTemp, Error, TEXT("Input Enabled"));
	
}

void ANHPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("TestRPC", IE_Pressed, this, &ANHPlayerController::TestInputRPC);
}

void ANHPlayerController::TestInputRPC_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("******************* Player Conroller Input Working ********************"))
}

void ANHPlayerController::ReportToClient_Implementation(const FString& Message)
{
	UE_LOG(LogTemp, Error, TEXT("******************* %s ********************"), *Message);
}

void ANHPlayerController::EndGame_Implementation(float Time, ENHTeams WinningTeam)
{
	GetPawn()->DisableInput(this);
	if(!HUD)
	{
		HUD = GetHUD<ANewHaloHUD>();
		if(!HUD)
		{
			UE_LOG(LogTemp, Error, TEXT("Cant Get HUD in: ANHPlayerController::EndGame"));
		}
	}
	if (HUD)
	{
		HUD->SetEndGameTimer(Time);
		HUD->ShowWinner(WinningTeam);
	}
}


