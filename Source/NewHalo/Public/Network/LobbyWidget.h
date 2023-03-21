// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/IHttpRequest.h"
#include "LobbyWidget.generated.h"

class UNHGameInstance;
enum class ENHGameModes;
class UTextBlock;
class FHttpModule;
class UComboBoxString;
class UButton;
class UWebBrowser;
/**
 * 
 */
UCLASS()
class NEWHALO_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddGameMode(ENHGameModes GameModeType, FString GameModeName);
	ULobbyWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnSettings();

	UFUNCTION()
	void OnExit();

	UFUNCTION()
	void OnChooseGameMode(FString SelectedGameMode, ESelectInfo::Type SelectionType);

private:
	UPROPERTY(meta=(BindWidget))
	UButton* PlayButton;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* PlayButtonText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* WelcomeText;

	UPROPERTY(meta=(BindWidget))
	UButton* ExitButton;

	UPROPERTY(meta=(BindWidget))
	UButton* SettingsButton;

	UPROPERTY(meta=(BindWidget))
	UComboBoxString* ModesDropDown;


	UPROPERTY(meta=(BindWidget))
	UWebBrowser* LoginMenuWebBrowser;

	UPROPERTY()
	UNHGameInstance* NHGameInstance;

	ENHGameModes ChosenGameMode;

	UPROPERTY()
	TMap<FString, ENHGameModes> ModeNameTypeMap;

	UPROPERTY()
	FString ApiUrl;
	UPROPERTY()
	FString CallbackUrl;

	UPROPERTY()
	FString LoginURL;

	////	

public:
	UPROPERTY()
	FTimerHandle SetAveragePlayerLatencyHandle;

	UPROPERTY()
	FTimerHandle PollMatchmakingHandle;


private:
	FHttpModule* HttpModule;

	UPROPERTY()
	FString RegionCode;


	UPROPERTY(meta=(BindWidget))
	UTextBlock* WinsTextBlock;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* LossesTextBlock;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* PingTextBlock;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* MatchmakingEventTextBlock;

	UPROPERTY()
	float AveragePlayerLatency;

	UPROPERTY()
	bool SearchingForGame;


	UFUNCTION()
	void HandleLoginUrlChange();

	UFUNCTION()
	void SetAveragePlayerLatency();

	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
	void PollMatchmaking();

	void OnExchangeCodeForTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
	                                             bool bWasSuccessful);
	void OnGetPlayerDataResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnStartMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnStopMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnPollMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
