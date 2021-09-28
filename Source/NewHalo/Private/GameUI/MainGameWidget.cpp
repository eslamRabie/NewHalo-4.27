// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUI/MainGameWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "SlateCore/Public/Styling/SlateColor.h"
#include "Player/NHPlayerState.h"

void UMainGameWidget::UpdateHealth(float HealthPercentage)
{
	Health->SetPercent(HealthPercentage);
}

void UMainGameWidget::SetInventoryItemIcon(UTexture2D* Icon, int32 SlotIndex)
{
	auto Wid = Cast<UImage>(Inventory->GetChildAt(SlotIndex));
	Wid->SetBrushFromTexture(Icon);
}

void UMainGameWidget::SetInventoryItemCount(int32 Count, int32 SlotIndex)
{
}

void UMainGameWidget::SetWeapon1Icon(UTexture2D* Icon)
{
	if(!Icon) return;
	Weapon1Icon->SetBrushFromTexture(Icon);
}

void UMainGameWidget::SetWeapon2Icon(UTexture2D* Icon)
{
	if(!Icon) return;
	Weapon2Icon->SetBrushFromTexture(Icon);
}

void UMainGameWidget::SetSmallWeaponIcon(UTexture2D* Icon)
{
	if(!Icon) return;
	SmallWeaponIcon->SetBrushFromTexture(Icon);
}

void UMainGameWidget::SetWeapon1Ammo(int32 NewAmmo)
{
	Weapon1Ammo->SetText(FText::AsNumber(NewAmmo));
}

void UMainGameWidget::SetWeapon2Ammo(int32 NewAmmo)
{
	Weapon2Ammo->SetText(FText::AsNumber(NewAmmo));
}

void UMainGameWidget::SetSmallWeaponAmmo(int32 NewAmmo)
{
	SmallWeaponAmmo->SetText(FText::AsNumber(NewAmmo));
}

void UMainGameWidget::SetControlPointTeam(ETeams Team)
{
	if(Team == ETeams::None)
	{
		ControllerTeam->SetBrushTintColor(FSlateColor(FLinearColor::White)); 
	}
	else if(Team == ETeams::BlueTeam)
	{
		ControllerTeam->SetBrushTintColor(FSlateColor(FLinearColor::Blue)); 
	}
	else if(Team == ETeams::RedTeam)
	{
		ControllerTeam->SetBrushTintColor(FSlateColor(FLinearColor::Red)); 
	}
}

void UMainGameWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if(!Health || !ControllerTeam || !Weapon1Ammo || !Weapon2Ammo || !SmallWeaponAmmo || !Weapon1Icon || ! Weapon2Icon ||
		!SmallWeaponIcon || !Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("Set vars in BP in %s"), *GetName());
		
		return;
	}
	SetControlPointTeam(ETeams::None);
}
