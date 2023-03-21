// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUI/NotificationMessageWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"


void UNotificationMessageWidget::SetMessage(FString Message)
{
	UTextBlock* TextBlock = WidgetTree->ConstructWidget<UTextBlock>();
	if(!TextBlock) return;
	TextBlock->SetText(FText::FromString(Message));
	
	auto OLP = GetOwningLocalPlayer();
	if(OLP && GetWorld())
	{
		auto PC = OLP->GetPlayerController(GetWorld());
		if(PC)
		{
			PC->GetWorldTimerManager().SetTimer(TimerHandle, this, &UUserWidget::RemoveFromParent, 1);
		}	
	}
	
}
