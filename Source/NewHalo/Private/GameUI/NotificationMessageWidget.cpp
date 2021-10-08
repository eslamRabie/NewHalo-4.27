// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUI/NotificationMessageWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"


void UNotificationMessageWidget::SetMessage(FString Message)
{
	UTextBlock* TextBlock = WidgetTree->ConstructWidget<UTextBlock>();
	TextBlock->SetText(FText::FromString(Message));
	GetOwningLocalPlayer()->GetPlayerController(GetWorld())->GetWorldTimerManager().SetTimer(TimerHandle, this, &UUserWidget::RemoveFromParent, 1);
}
