// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NotificationMessageWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "NotificationWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEWHALO_API UNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddMessage(FString Message);

private:
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* NotificationsArea;

	FTimerHandle MessageTimer;
};
