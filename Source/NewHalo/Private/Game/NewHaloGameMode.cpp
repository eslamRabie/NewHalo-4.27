// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/NewHaloGameMode.h"

#include "HttpModule.h"
#include "Game/NHGameStateBase.h"
#include "GameUI/NewHaloHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Player/NHPlayerController.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "aws/gamelift/server/GameLiftServerAPI.h"
#include "Game/NHControlGameMode.h"
#include "Game/NHControlGameState.h"
#include "Network/TextReaderActorComponent.h"
#include "Player/NewHaloCharacter.h"


ANewHaloGameMode::ANewHaloGameMode()
{
	// use our custom HUD class
	HUDClass = ANewHaloHUD::StaticClass();
	MatchStartWaitingTimeInS = 10;
	MatchEndWaitingTimeInS = 5;
	BlueTeamCount = 0;
	RedTeamCount = 0;

	UTextReaderActorComponent* TextReader = CreateDefaultSubobject<UTextReaderActorComponent>(TEXT("TextReaderComp"));
	ApiUrl = TextReader->ReadFile("URLs/ApiUrl.txt");

	HttpModule = &FHttpModule::Get();
	
	GameSessionActivated = false;

	WaitingForPlayersToJoin = false;
	TimeSpentWaitingForPlayersToJoin = 0;
	
}

void ANewHaloGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ANewHaloGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("BeginPlay in: %s"), *GetName());
	
	//
	//
	// UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerStart::StaticClass(), FName("RedTeam"),
	//                                              RedTeamPlayerStarts);
	// UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerStart::StaticClass(), FName("BlueTeam"),
	//                                              BlueTeamPlayerStarts);

#if WITH_GAMELIFT
	auto InitSDKOutcome = Aws::GameLift::Server::InitSDK();

	if (InitSDKOutcome.IsSuccess()) {
		auto OnStartGameSession = [](Aws::GameLift::Server::Model::GameSession GameSessionObj, void* Params)
		{
			FStartGameSessionState* State = (FStartGameSessionState*)Params;

			State->Status = Aws::GameLift::Server::ActivateGameSession().IsSuccess();

			FString MatchmakerData = GameSessionObj.GetMatchmakerData();

			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(MatchmakerData);

			if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
				State->MatchmakingConfigurationArn = JsonObject->GetStringField("matchmakingConfigurationArn");

				TArray<TSharedPtr<FJsonValue>> Teams = JsonObject->GetArrayField("teams");
				for (TSharedPtr<FJsonValue> Team : Teams) {
					TSharedPtr<FJsonObject> TeamObj = Team->AsObject();
					FString TeamName = TeamObj->GetStringField("name");

					TArray<TSharedPtr<FJsonValue>> Players = TeamObj->GetArrayField("players");

					for (TSharedPtr<FJsonValue> Player : Players) {
						TSharedPtr<FJsonObject> PlayerObj = Player->AsObject();
						FString PlayerId = PlayerObj->GetStringField("playerId");

						//TSharedPtr<FJsonObject> Attributes = PlayerObj->GetObjectField("attributes");
						//TSharedPtr<FJsonObject> PlayerGameMode = Attributes->GetObjectField("GameMode");
						//FString ModeValue = PlayerGameMode->GetStringField("valueAttribute");
						//auto ModeAttributeValue = new Aws::GameLift::Server::Model::AttributeValue(FCString::Atod(*ModeValue));

						Aws::GameLift::Server::Model::Player AwsPlayerObj;

						AwsPlayerObj.SetPlayerId(TCHAR_TO_ANSI(*PlayerId));
						AwsPlayerObj.SetTeam(TCHAR_TO_ANSI(*TeamName));
						//AwsPlayerObj.AddPlayerAttribute("GameMode", *ModeAttributeValue);

						State->PlayerIdToPlayer.Add(PlayerId, AwsPlayerObj);
					}
				}
			}
		};

		auto OnUpdateGameSession = [](Aws::GameLift::Server::Model::UpdateGameSession UpdateGameSessionObj, void* Params)
		{
			FUpdateGameSessionState* State = (FUpdateGameSessionState*)Params;

			auto Reason = UpdateGameSessionObj.GetUpdateReason();
			
		//	UE_LOG(LogTemp, Error, TEXT("*************** OnUpdateGameSession %s **************"), *UEnum::GetValueAsString(Reason));

			if (Reason == Aws::GameLift::Server::Model::UpdateReason::MATCHMAKING_DATA_UPDATED) {
				State->Reason = EUpdateReason::MATCHMAKING_DATA_UPDATED;

				auto GameSessionObj = UpdateGameSessionObj.GetGameSession();
				FString MatchmakerData = GameSessionObj.GetMatchmakerData();

				TSharedPtr<FJsonObject> JsonObject;
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(MatchmakerData);

				if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
					TArray<TSharedPtr<FJsonValue>> Teams = JsonObject->GetArrayField("teams");
					for (TSharedPtr<FJsonValue> Team : Teams) {
						TSharedPtr<FJsonObject> TeamObj = Team->AsObject();
						FString TeamName = TeamObj->GetStringField("name");

						TArray<TSharedPtr<FJsonValue>> Players = TeamObj->GetArrayField("players");

						for (TSharedPtr<FJsonValue> Player : Players) {
							TSharedPtr<FJsonObject> PlayerObj = Player->AsObject();
							FString PlayerId = PlayerObj->GetStringField("playerId");

							//TSharedPtr<FJsonObject> Attributes = PlayerObj->GetObjectField("attributes");
							//TSharedPtr<FJsonObject> PlayerGameMode = Attributes->GetObjectField("GameMode");
							//FString ModeValue = PlayerGameMode->GetStringField("valueAttribute");
							//auto ModeAttributeValue = new Aws::GameLift::Server::Model::AttributeValue(FCString::Atod(*ModeValue));

							Aws::GameLift::Server::Model::Player AwsPlayerObj;

							AwsPlayerObj.SetPlayerId(TCHAR_TO_ANSI(*PlayerId));
							AwsPlayerObj.SetTeam(TCHAR_TO_ANSI(*TeamName));
							//AwsPlayerObj.AddPlayerAttribute("GameMode", *ModeAttributeValue);

							State->PlayerIdToPlayer.Add(PlayerId, AwsPlayerObj);
						}
					}
				}
			}
			else if (Reason == Aws::GameLift::Server::Model::UpdateReason::BACKFILL_CANCELLED) {
				State->Reason = EUpdateReason::BACKFILL_CANCELLED;
			}
			else if (Reason == Aws::GameLift::Server::Model::UpdateReason::BACKFILL_FAILED) {
				State->Reason = EUpdateReason::BACKFILL_FAILED;
			}
			else if (Reason == Aws::GameLift::Server::Model::UpdateReason::BACKFILL_TIMED_OUT) {
				State->Reason = EUpdateReason::BACKFILL_TIMED_OUT;
			}
		};

		auto OnProcessTerminate = [](void* Params)
		{
			FProcessTerminateState* State = (FProcessTerminateState*)Params;

			auto GetTerminationTimeOutcome = Aws::GameLift::Server::GetTerminationTime();
			if (GetTerminationTimeOutcome.IsSuccess()) {
				State->TerminationTime = GetTerminationTimeOutcome.GetResult();
			}

			State->Status = true;
		};

		auto OnHealthCheck = [](void* Params)
		{
			FHealthCheckState* State = (FHealthCheckState*)Params;
			State->Status = true;
			return State->Status;
		};

		TArray<FString> CommandLineTokens;
		TArray<FString> CommandLineSwitches;
		int Port = FURL::UrlConfig.DefaultPort;

		// GameLiftTutorialServer.exe token -port=7777
		FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);


		for (FString Str : CommandLineSwitches) {
			FString Key;
			FString Value;
			if (Str.Split("=", &Key, &Value)) {
				if (Key.Equals("port")) {
					Port = FCString::Atoi(*Value);
				}
				else if (Key.Equals("password")) {
					ServerPassword = Value;
				}
			}
		}

		const char* LogFile = "aLogFile.txt";
		const char** LogFiles = &LogFile;
		auto LogParams = new Aws::GameLift::Server::LogParameters(LogFiles, 1);

		auto Params = new Aws::GameLift::Server::ProcessParameters(
			OnStartGameSession,
			&StartGameSessionState,
			OnUpdateGameSession,
			&UpdateGameSessionState,
			OnProcessTerminate,
			&ProcessTerminateState,
			OnHealthCheck,
			&HealthCheckState,
			Port,
			*LogParams
		);

		auto ProcessReadyOutcome = Aws::GameLift::Server::ProcessReady(*Params);
	}
#endif

	GetWorldTimerManager().SetTimer(HandleGameSessionUpdateHandle, this, &ANHControlGameMode::HandleGameSessionUpdate, 1.0f, true, 5.0f);
	GetWorldTimerManager().SetTimer(HandleProcessTerminationHandle, this, &ANHControlGameMode::HandleProcessTermination, 1.0f, true, 5.0f);


	
}


void ANewHaloGameMode::EndMatch()
{
	UE_LOG(LogTemp, Error, TEXT("*************** EndMatch Successful  **************"));
	GetWorldTimerManager().ClearTimer(EndGameHandle);
	GetWorldTimerManager().ClearTimer(HandleProcessTerminationHandle);
	GetWorldTimerManager().ClearTimer(HandleGameSessionUpdateHandle);
	GetWorldTimerManager().ClearTimer(SuspendBackfillHandle);
#if WITH_GAMELIFT
	Aws::GameLift::Server::ProcessEnding();
	Aws::GameLift::Server::ProcessEnding();
	FGenericPlatformMisc::RequestExit(false);
#endif
	Super::EndMatch();
}

void ANewHaloGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueNetId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueNetId, ErrorMessage);
	UE_LOG(LogTemp, Error, TEXT("PreLogin"));
#if WITH_GAMELIFT
	if (Options.Len() > 0) {
		const FString& PlayerSessionId = UGameplayStatics::ParseOption(Options, "PlayerSessionId");
		const FString& PlayerId = UGameplayStatics::ParseOption(Options, "PlayerId");
		UE_LOG(LogTemp, Error, TEXT("*************** Prelogin Successful:::: %s, %s  **************"), *PlayerSessionId, *PlayerId);
		if (PlayerSessionId.Len() > 0 && PlayerId.Len() > 0) {
			Aws::GameLift::Server::Model::DescribePlayerSessionsRequest DescribePlayerSessionsRequest;
			DescribePlayerSessionsRequest.SetPlayerSessionId(TCHAR_TO_ANSI(*PlayerSessionId));

			auto DescribePlayerSessionsOutcome = Aws::GameLift::Server::DescribePlayerSessions(DescribePlayerSessionsRequest);
			if (DescribePlayerSessionsOutcome.IsSuccess()) {
				auto DescribePlayerSessionsResult = DescribePlayerSessionsOutcome.GetResult();
				int Count;
				auto PlayerSessions = DescribePlayerSessionsResult.GetPlayerSessions(Count);
				if (PlayerSessions != nullptr) {
					auto PlayerSession = PlayerSessions[0];
					FString ExpectedPlayerId = PlayerSession.GetPlayerId();
					auto PlayerStatus = PlayerSession.GetStatus();

					if (ExpectedPlayerId.Equals(PlayerId) && PlayerStatus == Aws::GameLift::Server::Model::PlayerSessionStatus::RESERVED) {
						auto AcceptPlayerSessionOutcome = Aws::GameLift::Server::AcceptPlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));

	
						if (!AcceptPlayerSessionOutcome.IsSuccess()) {
							UE_LOG(LogTemp, Error, TEXT("*************** Un----Successful AcceptPlayerSessionOutcome    **************"));
							ErrorMessage = "Unauthorized";
						}
					}
					else {
						UE_LOG(LogTemp, Error, TEXT("*************** Unauthorized  **************"));
						ErrorMessage = "Unauthorized";
					}
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("*************** Unauthorized  **************"));
					ErrorMessage = "Unauthorized";
				}
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("*************** Unauthorized  **************"));
				ErrorMessage = "Unauthorized";
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("*************** Unauthorized  **************"));
			ErrorMessage = "Unauthorized";
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("*************** Unauthorized  **************"));
		ErrorMessage = "Unauthorized";
	}
#endif
}

void ANewHaloGameMode::Logout(AController* Exiting)
{
#if WITH_GAMELIFT
	if (LatestBackfillTicketId.Len() > 0) {
		auto GameSessionIdOutcome = Aws::GameLift::Server::GetGameSessionId();
		if (GameSessionIdOutcome.IsSuccess()) {
			FString GameSessionId = GameSessionIdOutcome.GetResult();
			FString MatchmakingConfigurationArn = StartGameSessionState.MatchmakingConfigurationArn;
			StopBackfillRequest(GameSessionId, MatchmakingConfigurationArn, LatestBackfillTicketId);
		}
	}
	if (Exiting != nullptr) {
		ANHPlayerState* NHPlayerState = Exiting->GetPlayerState<ANHPlayerState>();
		if (NHPlayerState != nullptr) {
			const FString& PlayerSessionId = NHPlayerState->PlayerSessionId;
			if (PlayerSessionId.Len() > 0) {
				Aws::GameLift::Server::RemovePlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));
			}
		}
	}
#endif
	Super::Logout(Exiting);
}

void ANewHaloGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogTemp, Error, TEXT("PostLogin"));
	auto PlayerC = Cast<ANHPlayerController>(NewPlayer);
	if(PlayerC)
	{
		PlayerC->ReportToClient("PostLogin");
		PlayerControllers.Add(PlayerC);
		
	}
	
}

void ANewHaloGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	for (auto Player : PlayerControllers)
	{
		if(Player)
		{
			RestartPlayer(Player);
		}
		
	}
}


FString ANewHaloGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueNetId,
                                        const FString& Options, const FString& Portal)
{
	FString InitializedString =  Super::InitNewPlayer(NewPlayerController, UniqueNetId, Options, Portal);
	ANHPlayerState* NPlayerState = NewPlayerController->GetPlayerState<ANHPlayerState>();
	auto PC = Cast<ANHPlayerController>(NewPlayerController);


#if WITH_GAMELIFT
	const FString& PlayerSessionId = UGameplayStatics::ParseOption(Options, "PlayerSessionId");
	const FString& PlayerId = UGameplayStatics::ParseOption(Options, "PlayerId");

	if (NewPlayerController)
	{
		ANHPlayerState* NHPlayerState = NewPlayerController->GetPlayerState<ANHPlayerState>();
		if (NHPlayerState)
		{
			NHPlayerState->PlayerSessionId = *PlayerSessionId;
			NHPlayerState->MatchmakingPlayerId = *PlayerId;

			if (UpdateGameSessionState.PlayerIdToPlayer.Num() > 0)
			{
				if (UpdateGameSessionState.PlayerIdToPlayer.Contains(PlayerId))
				{
					auto PlayerObj = UpdateGameSessionState.PlayerIdToPlayer.Find(PlayerId);
					FString Team = PlayerObj->GetTeam();
					NHPlayerState->Team = *Team;
					if (Team.Equals("Blue"))
					{
						NewPlayerController->GetPlayerState<ANHPlayerState>()->SetPlayerTeam(ENHTeams::BlueTeam);
						UE_LOG(LogTemp, Error, TEXT("Blue  ************ InitNewPlayer  ***************"));

						// if (BlueTeamPlayerStarts.Num() > 0)
						// {
						// 	auto PStart = BlueTeamPlayerStarts[0];
						// 	if (PStart)
						// 	{
						// 		RestartPlayerAtPlayerStart(NewPlayerController, PStart);
						// 	}
						// }
						// BlueTeamCount++;
					}
					else if (Team.Equals("Red"))
					{
						UE_LOG(LogTemp, Error, TEXT("Red  ************ InitNewPlayer  ***************"));
						NewPlayerController->GetPlayerState<ANHPlayerState>()->SetPlayerTeam(ENHTeams::RedTeam);
						// if (RedTeamPlayerStarts.Num() > 0)
						// {
						// 	auto PStart = RedTeamPlayerStarts[0];
						// 	if (PStart)
						// 	{
						// 		RestartPlayerAtPlayerStart(NewPlayerController, PStart);
						// 	}
						// }
						// RedTeamCount++;
					}
				}
			}
			else if (StartGameSessionState.PlayerIdToPlayer.Num() > 0)
			{
				if (StartGameSessionState.PlayerIdToPlayer.Contains(PlayerId))
				{
					UE_LOG(LogTemp, Error, TEXT("StartGameSessionState.PlayerIdToPlayer.Num() > 0 &&  StartGameSessionState.PlayerIdToPlayer.Contains(PlayerId) ************ InitNewPlayer  ***************"));
					auto PlayerObj = StartGameSessionState.PlayerIdToPlayer.Find(PlayerId);
					FString Team = PlayerObj->GetTeam();
					NHPlayerState->Team = *Team;
					if (Team.Equals("Blue"))
					{
						UE_LOG(LogTemp, Error, TEXT("Blue  ************ InitNewPlayer  ***************"));
						NewPlayerController->GetPlayerState<ANHPlayerState>()->SetPlayerTeam(ENHTeams::BlueTeam);
						// if (BlueTeamPlayerStarts.Num() > 0)
						// {
						// 	auto PStart = BlueTeamPlayerStarts[0];
						// 	if (PStart)
						// 	{
						// 		RestartPlayerAtPlayerStart(NewPlayerController, PStart);
						// 	}
						// }
						// BlueTeamCount++;
					}
					else if (Team.Equals("Red"))
					{
						UE_LOG(LogTemp, Error, TEXT("Red  ************ InitNewPlayer  ***************"));
						NewPlayerController->GetPlayerState<ANHPlayerState>()->SetPlayerTeam(ENHTeams::RedTeam);
						// if (RedTeamPlayerStarts.Num() > 0)
						// {
						// 	auto PStart = RedTeamPlayerStarts[0];
						// 	if (PStart)
						// 	{
						// 		RestartPlayerAtPlayerStart(NewPlayerController, PStart);
						// 	}
						// }
						// RedTeamCount++;
					}
				}
			}
		}
	}
#endif
	return InitializedString;
}

void ANewHaloGameMode::HandleProcessTermination()
{
	if (ProcessTerminateState.Status)
	{
		UE_LOG(LogTemp, Error, TEXT("************  Process WILL TERMINATE  ***************"));
		GetWorldTimerManager().ClearTimer(HandleProcessTerminationHandle);
		GetWorldTimerManager().ClearTimer(HandleGameSessionUpdateHandle);
		GetWorldTimerManager().ClearTimer(SuspendBackfillHandle);

#if WITH_GAMELIFT
		if (LatestBackfillTicketId.Len() > 0) {
			auto GameSessionIdOutcome = Aws::GameLift::Server::GetGameSessionId();
			if (GameSessionIdOutcome.IsSuccess()) {
				FString GameSessionArn = FString(GameSessionIdOutcome.GetResult());
				FString MatchmakingConfigurationArn = StartGameSessionState.MatchmakingConfigurationArn;
				StopBackfillRequest(GameSessionArn, MatchmakingConfigurationArn, LatestBackfillTicketId);
			}
		}
#endif
		
		GetWorldTimerManager().SetTimer(EndGameHandle, this, &ANewHaloGameMode::EndMatch, 1.0f, false, 2.0f);
	}
}

void ANewHaloGameMode::HandleGameSessionUpdate()
{
#if WITH_GAMELIFT
	if (!GameSessionActivated)
	{
		if (StartGameSessionState.Status)
		{
			GameSessionActivated = true;
			ExpectedPlayers = StartGameSessionState.PlayerIdToPlayer;
			WaitingForPlayersToJoin = true;
		}
	}
	else if (WaitingForPlayersToJoin) {
		UE_LOG(LogTemp, Error, TEXT("(*** WaitingForPlayersToJoin in HandleGameSessionUpdate ***"));
		if (TimeSpentWaitingForPlayersToJoin < 120) {
			UE_LOG(LogTemp, Error, TEXT("*** TimeSpentWaitingForPlayersToJoin < 120 HandleGameSessionUpdate : %d ***"), TimeSpentWaitingForPlayersToJoin);
			auto GameSessionIdOutcome = Aws::GameLift::Server::GetGameSessionId();
			if (GameSessionIdOutcome.IsSuccess()) {
				UE_LOG(LogTemp, Error, TEXT("*** GameSessionIdOutcome.IsSuccess() HandleGameSessionUpdate ***"));

				FString GameSessionId = FString(GameSessionIdOutcome.GetResult());

				Aws::GameLift::Server::Model::DescribePlayerSessionsRequest DescribePlayerSessionsRequest;
				DescribePlayerSessionsRequest.SetGameSessionId(TCHAR_TO_ANSI(*GameSessionId));
				DescribePlayerSessionsRequest.SetPlayerSessionStatusFilter("RESERVED");

				auto DescribePlayerSessionsOutcome = Aws::GameLift::Server::DescribePlayerSessions(DescribePlayerSessionsRequest);
				if (DescribePlayerSessionsOutcome.IsSuccess()) {
					UE_LOG(LogTemp, Error, TEXT("*** DescribePlayerSessionsOutcome.IsSuccess() HandleGameSessionUpdate ***"));

					auto DescribePlayerSessionsResult = DescribePlayerSessionsOutcome.GetResult();
					int Count;
					DescribePlayerSessionsResult.GetPlayerSessions(Count);
					if (Count == 0) {
						UE_LOG(LogTemp, Error, TEXT("*** Count == 0 BACKFILL_COMPLETED HandleGameSessionUpdate ***"));

						UpdateGameSessionState.Reason = EUpdateReason::BACKFILL_COMPLETED;

						WaitingForPlayersToJoin = false;
						TimeSpentWaitingForPlayersToJoin = 0;
					}
					else {
						TimeSpentWaitingForPlayersToJoin++;
					}
				}
				else {
					TimeSpentWaitingForPlayersToJoin++;
				}
			}
			else {
				TimeSpentWaitingForPlayersToJoin++;
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT(" *** ERROR BACKFILL_COMPLETED TimeOUt in HandleGameSessionUpdate ****"));

			UpdateGameSessionState.Reason = EUpdateReason::BACKFILL_COMPLETED;

			WaitingForPlayersToJoin = false;
			TimeSpentWaitingForPlayersToJoin = 0;
		}
	}
	else if (UpdateGameSessionState.Reason == EUpdateReason::MATCHMAKING_DATA_UPDATED) {
		UE_LOG(LogTemp, Error, TEXT("*** MATCHMAKING_DATA_UPDATED HandleGameSessionUpdate ***"));

		LatestBackfillTicketId = "";
		ExpectedPlayers = UpdateGameSessionState.PlayerIdToPlayer;

		WaitingForPlayersToJoin = true;
	}
	else if (UpdateGameSessionState.Reason == EUpdateReason::BACKFILL_CANCELLED || UpdateGameSessionState.Reason == EUpdateReason::BACKFILL_COMPLETED
		|| UpdateGameSessionState.Reason == EUpdateReason::BACKFILL_FAILED || UpdateGameSessionState.Reason == EUpdateReason::BACKFILL_TIMED_OUT)
	{
		UE_LOG(LogTemp, Error, TEXT("*** BACKFILL_CANCELLED HandleGameSessionUpdate ***"));
	
		LatestBackfillTicketId = "";

		TArray<APlayerState*> PlayerStates = GetWorld()->GetGameState()->PlayerArray;

		TMap<FString, Aws::GameLift::Server::Model::Player> ConnectedPlayers;
		for (APlayerState* PlayerState : PlayerStates) {
			if (PlayerState != nullptr) {
				auto NHPlayerState = Cast<ANHPlayerState>(PlayerState);
				if (NHPlayerState != nullptr) {
					auto PlayerObj = ExpectedPlayers.Find(NHPlayerState->MatchmakingPlayerId);
					if (PlayerObj != nullptr) {
						ConnectedPlayers.Add(NHPlayerState->MatchmakingPlayerId, *PlayerObj);
					}
				}
			}
		}

		if (ConnectedPlayers.Num() == 0) {
			UE_LOG(LogTemp, Error, TEXT("*** ConnectedPlayers.Num() == 0 EndMatch in HandleGameSessionUpdate ***"));
			EndMatch();
		}
		else if (ConnectedPlayers.Num() < 3) {
			UE_LOG(LogTemp, Error, TEXT(" *** ConnectedPlayers.Num() < 2 in: HandleGameSessionUpdate ***"));

			auto GameSessionIdOutcome = Aws::GameLift::Server::GetGameSessionId();
			if (GameSessionIdOutcome.IsSuccess()) {
				UE_LOG(LogTemp, Error, TEXT("*** START GAME SESSION ***"));

				FString GameSessionId = FString(GameSessionIdOutcome.GetResult());
				FString MatchmakingConfigurationArn = StartGameSessionState.MatchmakingConfigurationArn;
				LatestBackfillTicketId = CreateBackfillRequest(GameSessionId, MatchmakingConfigurationArn, ConnectedPlayers);
				if (LatestBackfillTicketId.Len() > 0) {
					UpdateGameSessionState.Reason = EUpdateReason::BACKFILL_INITIATED;
				}
			}
		}
	}
#endif
}

void ANewHaloGameMode::SuspendBackfill()
{
	GetWorldTimerManager().ClearTimer(HandleGameSessionUpdateHandle);
#if WITH_GAMELIFT
	if (LatestBackfillTicketId.Len() > 0) {

		UE_LOG(LogTemp, Error, TEXT("************ SuspendBackfill ***************"));
		auto GameSessionIdOutcome = Aws::GameLift::Server::GetGameSessionId();
		if (GameSessionIdOutcome.IsSuccess()) {
			FString GameSessionId = GameSessionIdOutcome.GetResult();
			FString MatchmakingConfigurationArn = StartGameSessionState.MatchmakingConfigurationArn;
			if (!StopBackfillRequest(GameSessionId, MatchmakingConfigurationArn, LatestBackfillTicketId)) {
				GetWorldTimerManager().SetTimer(SuspendBackfillHandle, this, &ANewHaloGameMode::SuspendBackfill, 1.0f, false, 1.0f);
			}
		}
		else {
			GetWorldTimerManager().SetTimer(SuspendBackfillHandle, this, &ANewHaloGameMode::SuspendBackfill, 1.0f, false, 1.0f);
		}
	}
#endif
}

FString ANewHaloGameMode::CreateBackfillRequest(FString GameSessionArn, FString MatchmakingConfigurationArn,
	TMap<FString, Aws::GameLift::Server::Model::Player> Players)
{
#if WITH_GAMELIFT
	UE_LOG(LogTemp, Error, TEXT("************ CreateBackfillRequest ***************"));
	Aws::GameLift::Server::Model::StartMatchBackfillRequest StartMatchBackfillRequest;
	StartMatchBackfillRequest.SetGameSessionArn(TCHAR_TO_ANSI(*GameSessionArn));
	StartMatchBackfillRequest.SetMatchmakingConfigurationArn(TCHAR_TO_ANSI(*MatchmakingConfigurationArn));

	for (auto& Elem : Players) {
		auto PlayerObj = Elem.Value;
		StartMatchBackfillRequest.AddPlayer(PlayerObj);
	}

	auto StartMatchBackfillOutcome = Aws::GameLift::Server::StartMatchBackfill(StartMatchBackfillRequest);
	if (StartMatchBackfillOutcome.IsSuccess()) {
		return StartMatchBackfillOutcome.GetResult().GetTicketId();
	}
	else {
		return "";
	}
#endif
	return "";
}

bool ANewHaloGameMode::StopBackfillRequest(FString GameSessionArn, FString MatchmakingConfigurationArn,
	FString TicketId)
{
#if WITH_GAMELIFT
	UE_LOG(LogTemp, Error, TEXT("************ StopBackfillRequest ***************"));
	Aws::GameLift::Server::Model::StopMatchBackfillRequest StopMatchBackfillRequest;
	StopMatchBackfillRequest.SetGameSessionArn(TCHAR_TO_ANSI(*GameSessionArn));
	StopMatchBackfillRequest.SetMatchmakingConfigurationArn(TCHAR_TO_ANSI(*MatchmakingConfigurationArn));
	StopMatchBackfillRequest.SetTicketId(TCHAR_TO_ANSI(*TicketId));

	auto StopMatchBackfillOutcome = Aws::GameLift::Server::StopMatchBackfill(StopMatchBackfillRequest);

	return StopMatchBackfillOutcome.IsSuccess();
#endif
	return false;
}

void ANewHaloGameMode::OnRecordMatchResultResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bWasSuccessful)
{
	UE_LOG(LogTemp, Error, TEXT(" ******* EndMatch ************ OnRecordMatchResultResponseReceived ***************"));
	GetWorldTimerManager().SetTimer(EndGameHandle, this, &ANewHaloGameMode::EndMatch, 1.0f, false, 5.0f);
}


void ANewHaloGameMode::ApplyDamageToPlayer_Implementation(ANHPlayerState* ShooterPS, ANHPlayerState* TargetPS,
                                                          float Damage)
{
	if(!ShooterPS)
	{
		UE_LOG(LogTemp, Error, TEXT("ShooterPS is null in: %s"), *GetName())
		return;
	}
	if(!TargetPS)
	{
		UE_LOG(LogTemp, Error, TEXT("TargetPS is null in: %s"), *GetName())
		return;
	}
	TargetPS->ReduceHealth(ShooterPS, Damage);

}


void ANewHaloGameMode::SetWinner(ENHTeams WinningTeam)
{
	UE_LOG(LogTemp, Error, TEXT(" ******* EndMatch ************ SetWinner ***************"));
	
	auto GS = Cast<ANHControlGameState>(GameState);
	if(GS)
		GS->NotifyGameEnding(MatchEndWaitingTimeInS);
	

#if WITH_GAMELIFT
	if (GameState != nullptr)
		{
			TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
			FString WiningT = "Blue";
			if(WinningTeam == ENHTeams::BlueTeam)
			{
				WiningT = "Blue";
			}
			else if(WinningTeam == ENHTeams::RedTeam)
			{
				WiningT = "Red";
			}
			RequestObj->SetStringField("winningTeam", WiningT);

			auto GetGameSessionIdOutcome = Aws::GameLift::Server::GetGameSessionId();
			if (GetGameSessionIdOutcome.IsSuccess()) {
				RequestObj->SetStringField("gameSessionId", GetGameSessionIdOutcome.GetResult());

				FString RequestBody;
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
				if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) {
					auto RecordMatchResultRequest = HttpModule->CreateRequest();
					RecordMatchResultRequest->OnProcessRequestComplete().BindUObject(this, &ANewHaloGameMode::OnRecordMatchResultResponseReceived);
					RecordMatchResultRequest->SetURL(ApiUrl + "/recordmatchresult");
					RecordMatchResultRequest->SetVerb("POST");
					RecordMatchResultRequest->SetHeader("Authorization", ServerPassword);
					RecordMatchResultRequest->SetHeader("Content-Type", "application/json");
					RecordMatchResultRequest->SetContentAsString(RequestBody);
					RecordMatchResultRequest->ProcessRequest();
				}
				else {
					GetWorldTimerManager().SetTimer(StartEndTimerHandle, this, &ANewHaloGameMode::EndMatch, MatchEndWaitingTimeInS);
				}
			}
			else {
				GetWorldTimerManager().SetTimer(StartEndTimerHandle, this, &ANewHaloGameMode::EndMatch, MatchEndWaitingTimeInS);
			}
		}
		else {
			GetWorldTimerManager().SetTimer(StartEndTimerHandle, this, &ANewHaloGameMode::EndMatch, MatchEndWaitingTimeInS);
		}
#endif
	
	
}

void ANewHaloGameMode::AddKills_Implementation(ANHPlayerState* ShooterPS, ANHPlayerState* TargetPS)
{
	if(!ShooterPS)
	{
		UE_LOG(LogTemp, Error, TEXT("ShooterPS is Null in: %s"), *GetName());
		return;
	}
	if(!TargetPS)
	{
		UE_LOG(LogTemp, Error, TEXT("ShooterPS is Null in: %s"), *GetName());
		return;
	}
	ShooterPS->AddKills();
	auto NHGS = GetGameState<ANHGameStateBase>();
	NHGS->NotifyKill(ShooterPS, TargetPS);
	auto TargetPC = TargetPS->GetNetOwningPlayer()->GetPlayerController(GetWorld());

	auto PChar = TargetPC->GetPawn<ANewHaloCharacter>();
	auto Loc = PChar->GetActorLocation();
	auto Rot = PChar->GetActorRotation();
	// AActor* SpawnPoint = nullptr;
	// if(BlueTeamPlayerStarts.Num() <= 0)
	// {
	// 	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerStart::StaticClass(), FName("RedTeam"),
	// 											RedTeamPlayerStarts);
	// 	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerStart::StaticClass(), FName("BlueTeam"),
	// 												BlueTeamPlayerStarts);
	// }
	// if(TargetPS->GetPlayerTeam() == ENHTeams::BlueTeam)
	// {
	// 	if(BlueTeamPlayerStarts.Num() > 0)
	// 		SpawnPoint = BlueTeamPlayerStarts[0];
	// }
	// else
	// {
	// 	if(RedTeamPlayerStarts.Num() > 0)
	// 		SpawnPoint = RedTeamPlayerStarts[0];
	// }
	FActorSpawnParameters Sp;
	Sp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ANewHaloCharacter* NewPChar;
	// if(SpawnPoint)
	// {
	// 	NewPChar = GetWorld()->SpawnActor<ANewHaloCharacter>(PChar->GetClass(),SpawnPoint->GetActorLocation(),
	// 	SpawnPoint->GetActorRotation(), Sp);
	// }
	// else
	// {
	NewPChar = GetWorld()->SpawnActor<ANewHaloCharacter>(PChar->GetClass(),Loc, Rot, Sp);
	// }

	TargetPC->UnPossess();
	PChar->Destroy();
	TargetPS->Reset();
	TargetPC->Possess(NewPChar);
}


/////////////////////////////////////////////////////////////////////
///
