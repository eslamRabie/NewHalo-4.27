// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/LobbyWidget.h"

#include "NHGameInstance.h"
#include "Network/LobbyGameMode.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Network/LobbyPlayerController.h"
#include "WebBrowser.h"
#include "WebBrowserModule.h"
#include "IWebBrowserSingleton.h"
#include "IWebBrowserCookieManager.h"
#include "Network/TextReaderActorComponent.h"
#include "Http.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"


ULobbyWidget::ULobbyWidget(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	UTextReaderActorComponent* TextReaderActorComponent = CreateDefaultSubobject<UTextReaderActorComponent>(
		TEXT("TextReaderComponent"));
	if (!TextReaderActorComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("TextReaderActorComponent Can't be created!"));
		return;
	}
	LoginURL = TextReaderActorComponent->ReadFile("URLs/Login.txt");
	if (LoginURL.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("LoginURL is empty!"));
		return;
	}

	ApiUrl = TextReaderActorComponent->ReadFile("URLs/ApiUrl.txt");
	CallbackUrl = TextReaderActorComponent->ReadFile("URLs/CallbackUrl.txt");
	RegionCode = TextReaderActorComponent->ReadFile("URLs/RegionCode.txt");

	HttpModule = &FHttpModule::Get();

	AveragePlayerLatency = 60.0;

	SearchingForGame = false;
}


void ULobbyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ChosenGameMode = ENHGameModes::Control;

	if (!PlayButton)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayButton Was not set In BP!"));
		return;
	}
	PlayButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnPlayButtonClicked);

	if (!SettingsButton)
	{
		UE_LOG(LogTemp, Error, TEXT("SettingsButton Was not set In BP!"));
		return;
	}
	SettingsButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnSettings);

	if (!ExitButton)
	{
		UE_LOG(LogTemp, Error, TEXT("ExitButton Was not set In BP!"));
		return;
	}
	ExitButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnExit);

	if (!ModesDropDown)
	{
		UE_LOG(LogTemp, Error, TEXT("ModesDropDown Was not set In BP!"));
		return;
	}
	ModesDropDown->OnSelectionChanged.AddDynamic(this, &ULobbyWidget::OnChooseGameMode);

	///
	///Login

	if (!LoginMenuWebBrowser)
	{
		UE_LOG(LogTemp, Error, TEXT("LoginMenuWebBrowser Was not set In BP!"));
		return;
	}

	auto WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
	if (!WebBrowserSingleton)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant get WebBrowserSingleton!"));
		return;
	}
	TOptional<FString> DefaultContext;
	auto CookieManager = WebBrowserSingleton->GetCookieManager(DefaultContext);
	if (!CookieManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Cant get CookieManager!"));
	}
	else
	{
		CookieManager->DeleteCookies();
	}
	//LoginMenuWebBrowser->LoadURL(LoginURL);

	NHGameInstance = GetGameInstance<UNHGameInstance>();
	if (!NHGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant get NHGameInstance in ULobbyWidget::NativeOnInitialized!"));
	}
}

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	/*if(LoginMenuWebBrowser && !LoginURL.IsEmpty())
	{
		LoginMenuWebBrowser->LoadURL(LoginURL);
	}*/
	GetWorld()->GetTimerManager().SetTimer(SetAveragePlayerLatencyHandle, this, &ULobbyWidget::SetAveragePlayerLatency,
	                                       1.0f, true, 1.0f);

	FString AccessToken;
	if (NHGameInstance)
	{
		AccessToken = NHGameInstance->AccessToken;
	}
	

	if (AccessToken.Len() > 0)
	{
		auto GetPlayerDataRequest = HttpModule->CreateRequest();
		GetPlayerDataRequest->OnProcessRequestComplete().BindUObject(
			this, &ULobbyWidget::OnGetPlayerDataResponseReceived);
		GetPlayerDataRequest->SetURL(ApiUrl + "/getplayerdata");
		GetPlayerDataRequest->SetVerb("GET");
		//GetPlayerDataRequest->SetHeader("Content-Type", "application/json");
		GetPlayerDataRequest->SetHeader("Authorization", AccessToken);
		GetPlayerDataRequest->ProcessRequest();
	}
	else
	{
		IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();

		if (WebBrowserSingleton)
		{
			TOptional<FString> DefaultContext;
			TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager(DefaultContext);
			if (CookieManager.IsValid())
			{
				CookieManager->DeleteCookies();
			}
		}

		LoginMenuWebBrowser->LoadURL(LoginURL);

		FScriptDelegate LoginDelegate;
		LoginDelegate.BindUFunction(this, "HandleLoginUrlChange");
		LoginMenuWebBrowser->OnUrlChanged.Add(LoginDelegate);
	}
}

void ULobbyWidget::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle);
	GetWorld()->GetTimerManager().ClearTimer(SetAveragePlayerLatencyHandle);
	Super::NativeDestruct();
}

void ULobbyWidget::OnSettings()
{
	// todo show Settings Menu
}

void ULobbyWidget::OnExit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void ULobbyWidget::OnChooseGameMode(FString SelectedGameMode, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Error, TEXT("%s"), *SelectedGameMode)
	uint8 Index = ModesDropDown->FindOptionIndex(SelectedGameMode);
	ChosenGameMode = static_cast<ENHGameModes>(Index);
	UE_LOG(LogTemp, Error, TEXT("%s"), *UEnum::GetValueAsString(ChosenGameMode));
}

void ULobbyWidget::AddGameMode(ENHGameModes GameModeType, FString GameModeName)
{
	ModesDropDown->AddOption(GameModeName);
	ModeNameTypeMap.Emplace(GameModeName, GameModeType);
}


void ULobbyWidget::HandleLoginUrlChange()
{
	FString BrowserUrl = LoginMenuWebBrowser->GetUrl();
	FString Url;
	FString QueryParameters;

	if (BrowserUrl.Split("?", &Url, &QueryParameters))
	{
		if (Url.Equals(CallbackUrl))
		{
			FString ParameterName;
			FString ParameterValue;

			if (QueryParameters.Split("=", &ParameterName, &ParameterValue))
			{
				if (ParameterName.Equals("code"))
				{
					ParameterValue = ParameterValue.Replace(*FString("#"), *FString(""));

					TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
					RequestObj->SetStringField(ParameterName, ParameterValue);

					FString RequestBody;
					TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

					if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
					{
						auto ExchangeCodeForTokensRequest = HttpModule->CreateRequest();
						ExchangeCodeForTokensRequest->OnProcessRequestComplete().BindUObject(
							this, &ULobbyWidget::OnExchangeCodeForTokensResponseReceived);
						ExchangeCodeForTokensRequest->SetURL(ApiUrl + "/exchangecodefortokens");
						ExchangeCodeForTokensRequest->SetVerb("POST");
						ExchangeCodeForTokensRequest->SetHeader("Content-Type", "application/json");
						ExchangeCodeForTokensRequest->SetContentAsString(RequestBody);
						ExchangeCodeForTokensRequest->ProcessRequest();
					}
				}
			}
		}
	}
}

void ULobbyWidget::OnExchangeCodeForTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
                                                           bool bWasSuccessful)
{
	UE_LOG(LogTemp, Error, TEXT("OnExchangeCodeForTokensResponseReceived"));

	if (!bWasSuccessful)
	{
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("OnExchangeCodeForTokensResponseReceived .... Successful"));


	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		if (JsonObject->HasField("access_token") && JsonObject->HasField("id_token") && JsonObject->HasField(
			"refresh_token"))
		{
			if (NHGameInstance)
			{
				FString AccessToken = JsonObject->GetStringField("access_token");
				FString IdToken = JsonObject->GetStringField("id_token");
				FString RefreshToken = JsonObject->GetStringField("refresh_token");
				NHGameInstance->SetCognitoTokens(AccessToken, IdToken, RefreshToken);

				auto GetPlayerDataRequest = HttpModule->CreateRequest();
				GetPlayerDataRequest->OnProcessRequestComplete().BindUObject(
					this, &ULobbyWidget::OnGetPlayerDataResponseReceived);
				GetPlayerDataRequest->SetURL(ApiUrl + "/getplayerdata");
				GetPlayerDataRequest->SetVerb("GET");
				//GetPlayerDataRequest->SetHeader("Content-Type", "application/json");
				GetPlayerDataRequest->SetHeader("Authorization", AccessToken);
				GetPlayerDataRequest->ProcessRequest();
			}
		}
	}
}

void ULobbyWidget::OnGetPlayerDataResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
                                                   bool bWasSuccessful)
{
	UE_LOG(LogTemp, Error, TEXT("OnGetPlayerDataResponseReceived .... Successful"));

	if (!bWasSuccessful)
	{
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("OnGetPlayerDataResponseReceived .... Successful"));


	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		if (!JsonObject->HasField("playerData"))
		{
			return;
		}
		TSharedPtr<FJsonObject> PlayerData = JsonObject->GetObjectField("playerData");
		TSharedPtr<FJsonObject> WinsObject = PlayerData->GetObjectField("Wins");
		TSharedPtr<FJsonObject> LossesObject = PlayerData->GetObjectField("Losses");
		TSharedPtr<FJsonObject> NameObject = PlayerData->GetObjectField("PlayerName");

		FString Wins = WinsObject->GetStringField("N");
		FString Losses = LossesObject->GetStringField("N");
		FString PlayerName = NameObject->GetStringField("S");

		auto PS = GetOwningPlayerState();
		PS->SetPlayerName(PlayerName);

		WelcomeText->SetText(FText::FromString("Welcome " + PlayerName));

		WinsTextBlock->SetText(FText::FromString("Wins: " + Wins));
		LossesTextBlock->SetText(FText::FromString("Losses: " + Losses));

		LoginMenuWebBrowser->SetVisibility(ESlateVisibility::Collapsed);
		PlayButton->SetVisibility(ESlateVisibility::Visible);
		WinsTextBlock->SetVisibility(ESlateVisibility::Visible);
		LossesTextBlock->SetVisibility(ESlateVisibility::Visible);
		PingTextBlock->SetVisibility(ESlateVisibility::Visible);
		MatchmakingEventTextBlock->SetVisibility(ESlateVisibility::Visible);
	}
}

void ULobbyWidget::SetAveragePlayerLatency()
{
	if (!NHGameInstance)
	{
		return;
	}
	float TotalPlayerLatency = 0.0f;
	for (float PlayerLatency : NHGameInstance->PlayerLatencies)
	{
		TotalPlayerLatency += PlayerLatency;
	}

	if (TotalPlayerLatency > 0)
	{
		AveragePlayerLatency = TotalPlayerLatency / NHGameInstance->PlayerLatencies.Num();
		FString PingString = "Ping: " + FString::FromInt(FMath::RoundToInt(AveragePlayerLatency)) + "ms";
		PingTextBlock->SetText(FText::FromString(PingString));
	}
}

void ULobbyWidget::OnPlayButtonClicked()
{
	PlayButton->SetIsEnabled(false);

	FString AccessToken;
	FString MatchmakingTicketId;
	if (NHGameInstance)
	{
		AccessToken = NHGameInstance->AccessToken;
		MatchmakingTicketId = NHGameInstance->MatchmakingTicketId;
	}

	if (SearchingForGame)
	{
		GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle);
		SearchingForGame = false;

		if (AccessToken.Len() > 0 && MatchmakingTicketId.Len() > 0)
		{
			TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
			RequestObj->SetStringField("ticketId", MatchmakingTicketId);

			FString RequestBody;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
			if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
			{
				auto StopMatchmakingRequest = HttpModule->CreateRequest();
				StopMatchmakingRequest->OnProcessRequestComplete().BindUObject(
					this, &ULobbyWidget::OnStopMatchmakingResponseReceived);
				StopMatchmakingRequest->SetURL(ApiUrl + "/stopmatchmaking");
				StopMatchmakingRequest->SetVerb("POST");
				StopMatchmakingRequest->SetHeader("Content-Type", "application/json");
				StopMatchmakingRequest->SetHeader("Authorization", AccessToken);
				StopMatchmakingRequest->SetContentAsString(RequestBody);
				StopMatchmakingRequest->ProcessRequest();
			}
			else
			{
				PlayButtonText->SetText(FText::FromString("Play"));
				MatchmakingEventTextBlock->SetText(FText::FromString(""));
				PlayButton->SetIsEnabled(true);
			}
		}
		else
		{
			PlayButtonText->SetText(FText::FromString("Play"));
			MatchmakingEventTextBlock->SetText(FText::FromString(""));
			PlayButton->SetIsEnabled(true);
		}
	}
	else
	{
		if (AccessToken.Len() > 0)
		{
			TSharedRef<FJsonObject> LatencyMapObj = MakeShareable(new FJsonObject);
			LatencyMapObj->SetNumberField(RegionCode, AveragePlayerLatency);

			TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
			RequestObj->SetObjectField("latencyMap", LatencyMapObj);

			FString RequestBody;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
			if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
			{
				auto StartMatchmakingRequest = HttpModule->CreateRequest();
				StartMatchmakingRequest->OnProcessRequestComplete().BindUObject(
					this, &ULobbyWidget::OnStartMatchmakingResponseReceived);
				StartMatchmakingRequest->SetURL(ApiUrl + "/startmatchmaking");
				StartMatchmakingRequest->SetVerb("POST");
				StartMatchmakingRequest->SetHeader("Content-Type", "application/json");
				StartMatchmakingRequest->SetHeader("Authorization", AccessToken);
				StartMatchmakingRequest->SetContentAsString(RequestBody);
				StartMatchmakingRequest->ProcessRequest();
			}
			else
			{
				PlayButton->SetIsEnabled(true);
			}
		}
		else
		{
			PlayButton->SetIsEnabled(true);
		}
	}
}

void ULobbyWidget::OnStartMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
                                                      bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if (JsonObject->HasField("ticketId"))
			{
				FString MatchmakingTicketId = JsonObject->GetStringField("ticketId");
				if (NHGameInstance)
				{
					NHGameInstance->MatchmakingTicketId = MatchmakingTicketId;

					GetWorld()->GetTimerManager().SetTimer(PollMatchmakingHandle, this, &ULobbyWidget::PollMatchmaking,
					                                       1.0f, true, 1.0f);
					SearchingForGame = true;

					PlayButtonText->SetText(FText::FromString("Cancel"));
					MatchmakingEventTextBlock->SetText(FText::FromString("Currently looking for a match"));
				}
			}
		}
	}
	PlayButton->SetIsEnabled(true);
}

void ULobbyWidget::OnStopMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
                                                     bool bWasSuccessful)
{
	if (NHGameInstance)
	{
		NHGameInstance->MatchmakingTicketId = "";
	}

	PlayButtonText->SetText(FText::FromString("Play"));
	MatchmakingEventTextBlock->SetText(FText::FromString(""));
	PlayButton->SetIsEnabled(true);
}

void ULobbyWidget::PollMatchmaking()
{
	FString AccessToken;
	FString MatchmakingTicketId;

	if (NHGameInstance)
	{
		AccessToken = NHGameInstance->AccessToken;
		MatchmakingTicketId = NHGameInstance->MatchmakingTicketId;
	}

	if (AccessToken.Len() > 0 && MatchmakingTicketId.Len() > 0 && SearchingForGame)
	{
		TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
		RequestObj->SetStringField("ticketId", MatchmakingTicketId);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
		if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
		{
			auto PollMatchmakingRequest = HttpModule->CreateRequest();
			PollMatchmakingRequest->OnProcessRequestComplete().BindUObject(
				this, &ULobbyWidget::OnPollMatchmakingResponseReceived);
			PollMatchmakingRequest->SetURL(ApiUrl + "/pollmatchmaking");
			PollMatchmakingRequest->SetVerb("POST");
			PollMatchmakingRequest->SetHeader("Content-Type", "application/json");
			PollMatchmakingRequest->SetHeader("Authorization", AccessToken);
			PollMatchmakingRequest->SetContentAsString(RequestBody);
			PollMatchmakingRequest->ProcessRequest();
		}
	}
}

void ULobbyWidget::OnPollMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
                                                     bool bWasSuccessful)
{
	if (bWasSuccessful && SearchingForGame)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			// FString OutputStringx;
			// TSharedRef< TJsonWriter<> > Writerx = TJsonWriterFactory<>::Create(&OutputStringx);
			// FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writerx);

			//UE_LOG(LogTemp, Error, TEXT("JsonObject: %s"), *OutputStringx);

			if (JsonObject->HasField("ticket"))
			{
				TSharedPtr<FJsonObject> Ticket = JsonObject->GetObjectField("ticket");
				FString TicketType = Ticket->GetObjectField("Type")->GetStringField("S");
				//UE_LOG(LogTemp, Error, TEXT("ticket"));

				if (TicketType.Len() > 0)
				{
					GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle);
					SearchingForGame = false;
					UE_LOG(LogTemp, Error, TEXT("TicketType.Len() > 0"));
					if (NHGameInstance)
					{
						NHGameInstance->MatchmakingTicketId = "";
					}

					//UE_LOG(LogTemp, Error, TEXT("options: %s"), *TicketType);


					if (TicketType.Equals("MatchmakingSucceeded"))
					{
						UE_LOG(LogTemp, Error, TEXT("MatchmakingSucceeded"));

						PlayButton->SetIsEnabled(false);
						MatchmakingEventTextBlock->SetText(
							FText::FromString("Successfully found a match. Now connecting to the server..."));

						TSharedPtr<FJsonObject> GameSessionInfo = Ticket->GetObjectField("GameSessionInfo")->
						                                                  GetObjectField("M");
						FString IpAddress = GameSessionInfo->GetObjectField("IpAddress")->GetStringField("S");
						FString Port = GameSessionInfo->GetObjectField("Port")->GetStringField("N");

						TArray<TSharedPtr<FJsonValue>> Players = Ticket->GetObjectField("Players")->GetArrayField("L");
						TSharedPtr<FJsonObject> Player = Players[0]->AsObject()->GetObjectField("M");
						FString PlayerSessionId = Player->GetObjectField("PlayerSessionId")->GetStringField("S");
						FString PlayerId = Player->GetObjectField("PlayerId")->GetStringField("S");

						FString LevelName = IpAddress + ":" + Port;
						const FString& Options =  "?PlayerSessionId=" + PlayerSessionId + "?PlayerId=" + PlayerId /*+ "?listen"*/;
						UE_LOG(LogTemp, Error, TEXT("options: %s"), *Options);

						UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName), false, Options);

						// auto Level = LevelName + Options;
						//
						// auto PC = GetWorld()->GetFirstPlayerController();
						// if(PC)
						// {
						// 	PC->ClientTravel(Level, ETravelType::TRAVEL_Relative);
						// }
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("MatchmakingFaild"));
						PlayButtonText->SetText(FText::FromString("Play"));
						MatchmakingEventTextBlock->SetText(FText::FromString(TicketType + ". Please try again"));
					}
				}
			}
		}
	}
}
