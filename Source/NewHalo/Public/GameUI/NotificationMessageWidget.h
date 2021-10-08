// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotificationMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEWHALO_API UNotificationMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMessage(FString Message);

private:
	FTimerHandle TimerHandle;
};
