// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUI/GameMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UGameMenuWidget::OnResume()
{
	RemoveFromViewport();
}

void UGameMenuWidget::OnSettings()
{
	// Show Settings Menu
}

void UGameMenuWidget::OnLeave()
{
	
}

void UGameMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if(!LeaveButton || !ResumeButton || !SettingsButton)
	{
		return;
	}
	ResumeButton->OnClicked.AddDynamic(this, &UGameMenuWidget::OnResume);
	SettingsButton->OnClicked.AddDynamic(this, &UGameMenuWidget::OnSettings);
	LeaveButton->OnClicked.AddDynamic(this, &UGameMenuWidget::OnLeave);
}
