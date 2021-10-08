// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUI/NotificationWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"

void UNotificationWidget::AddMessage(FString Message)
{
	auto NotMesWid = CreateWidget<UNotificationMessageWidget>(GetWorld());
	NotMesWid->SetMessage(Message);
	NotificationsArea->AddChildToVerticalBox(NotMesWid);
}
