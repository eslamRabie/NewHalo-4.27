// Fill out your copyright notice in the Description page of Project OnSettings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameLiftServerSDK.h"
#include "LobbyGameMode.generated.h"


class UNHGameInstance;
class ALobbyPlayerController;
UENUM()
enum class ENHGameModes
{
	Control,
	Assault,
	Demolition,
	SecureArea,
	CaptureTheFlag,
	Hostage,
	MAX
};


USTRUCT()
struct FGameModeInfo
{
	GENERATED_BODY()


	UPROPERTY(EditDefaultsOnly)
	ENHGameModes GameModeType;
	UPROPERTY(EditDefaultsOnly)
	FString GameModeName;
	UPROPERTY(EditDefaultsOnly)
	TAssetPtr<UWorld> Level;
	UPROPERTY(EditDefaultsOnly)
	FName LevelPath;
};

UCLASS()
class NEWHALO_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALobbyGameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;


	/// Online
	/// 

private:
	UPROPERTY(EditDefaultsOnly, Category=GameModes)
	TArray<FGameModeInfo> AvailableGameModes;
};
