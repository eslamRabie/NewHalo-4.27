// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUI/SettingsMenuWidget.h"

#include "Components/Button.h"

void USettingsMenuWidget::OnResume()
{
	RemoveFromViewport();
}

void USettingsMenuWidget::OnSettings()
{
	// Show Settings Menu
}

void USettingsMenuWidget::OnLeave()
{
}

void USettingsMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!LeaveButton || !ResumeButton || !SettingsButton)
	{
		return;
	}
	ResumeButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnResume);
	SettingsButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnSettings);
	LeaveButton->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnLeave);
}

