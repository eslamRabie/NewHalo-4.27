// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMenuWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class NEWHALO_API UGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnResume();

	UFUNCTION()
	void OnSettings();

	UFUNCTION()
	void OnLeave();

protected:
	virtual void NativeOnInitialized() override;
	
private:
	UPROPERTY(meta=(BindWidget))
    UButton* ResumeButton;
	UPROPERTY(meta=(BindWidget))
	UButton* SettingsButton;
	UPROPERTY(meta=(BindWidget))
	UButton* LeaveButton;


	
};
