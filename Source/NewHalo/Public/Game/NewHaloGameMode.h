// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NHGameSession.h"
#include "aws/gamelift/server/model/Player.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Interfaces/IHttpRequest.h"
#include "Player/NHPlayerState.h"
#include "NewHaloGameMode.generated.h"

class FHttpModule;


class UNHGameInstance;
class ANHPlayerController;

UENUM()
enum class EUpdateReason : uint8
{
	NO_UPDATE_RECEIVED,
	BACKFILL_INITIATED,
	MATCHMAKING_DATA_UPDATED,
	BACKFILL_FAILED,
	BACKFILL_TIMED_OUT,
	BACKFILL_CANCELLED,
	BACKFILL_COMPLETED
};

USTRUCT()
struct FStartGameSessionState
{
	GENERATED_BODY();

	UPROPERTY()
	bool Status;

	UPROPERTY()
	FString MatchmakingConfigurationArn;

	TMap<FString, Aws::GameLift::Server::Model::Player> PlayerIdToPlayer;

	FStartGameSessionState() {
		Status = false;
	}
};

USTRUCT()
struct FUpdateGameSessionState
{
	GENERATED_BODY();

	UPROPERTY()
	EUpdateReason Reason;

	TMap<FString, Aws::GameLift::Server::Model::Player> PlayerIdToPlayer;

	FUpdateGameSessionState() {
		Reason = EUpdateReason::NO_UPDATE_RECEIVED;
	}
};

USTRUCT()
struct FProcessTerminateState
{
	GENERATED_BODY();

	UPROPERTY()
	bool Status;

	long TerminationTime;

	FProcessTerminateState() {
		Status = false;
		TerminationTime = 0L;
	}
};

USTRUCT()
struct FHealthCheckState
{
	GENERATED_BODY();

	UPROPERTY()
	bool Status;

	FHealthCheckState() {
		Status = false;
	}
};


UCLASS()
class ANewHaloGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ANewHaloGameMode();

	virtual void Tick(float DeltaSeconds) override;
	

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void ApplyDamageToPlayer(ANHPlayerState* ShooterPS, ANHPlayerState* TargetPS, float Damage);

	UFUNCTION(BlueprintCallable)
	virtual void SetWinner(ENHTeams WinningTeam);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void AddKills(ANHPlayerState* ShooterPS, ANHPlayerState* TargetPS);


protected:
	virtual void BeginPlay() override;

	//virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void EndMatch() override;

	// Structs
	UPROPERTY()
	FStartGameSessionState StartGameSessionState;

	UPROPERTY()
	FUpdateGameSessionState UpdateGameSessionState;

	UPROPERTY()
	FProcessTerminateState ProcessTerminateState;

	UPROPERTY()
	FHealthCheckState HealthCheckState;

	
	UPROPERTY()
	TArray<ANHPlayerController*> PlayerControllers;


	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta=(AllowPrivateAccess), Category=GameDesign)
	float MatchStartWaitingTimeInS;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta=(AllowPrivateAccess), Category=GameDesign)
	float MatchEndWaitingTimeInS;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta=(AllowPrivateAccess), Category=GameDesign)
	float MaxWaitingTimeInSeconds;
	

	FTimerHandle StartEndTimerHandle;

	bool bCanStartMatch;

	bool bCanEndMatch;

	float CurrentWaitingTime;

	int32 BlueTeamCount;
	int32 RedTeamCount;





/////////////////////////////
///

protected:
	
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void Logout(AController* Exiting) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void HandleMatchHasStarted() override;

	

public:


	UPROPERTY()
		FTimerHandle EndGameHandle;

	UPROPERTY()
		FTimerHandle HandleProcessTerminationHandle;

	UPROPERTY()
		FTimerHandle HandleGameSessionUpdateHandle;

	UPROPERTY()
		FTimerHandle SuspendBackfillHandle;

protected:

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
	
	FHttpModule* HttpModule;

	UPROPERTY()
		FString ApiUrl;

	UPROPERTY()
		FString ServerPassword;

	// UPROPERTY()
	// 	int RemainingGameTime;

	UPROPERTY()
		bool GameSessionActivated;

	UPROPERTY()
		FString LatestBackfillTicketId;

	UPROPERTY()
		bool WaitingForPlayersToJoin;

	UPROPERTY()
		int TimeSpentWaitingForPlayersToJoin;

	TMap<FString, Aws::GameLift::Server::Model::Player> ExpectedPlayers;
	

	UFUNCTION()
		virtual void HandleProcessTermination();

	UFUNCTION()
		virtual void HandleGameSessionUpdate();

	UFUNCTION()
		virtual void SuspendBackfill();

	virtual FString CreateBackfillRequest(FString GameSessionArn, FString MatchmakingConfigurationArn, TMap<FString, Aws::GameLift::Server::Model::Player> Players);
	virtual bool StopBackfillRequest(FString GameSessionArn, FString MatchmakingConfigurationArn, FString TicketId);
	virtual void OnRecordMatchResultResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


	
};

