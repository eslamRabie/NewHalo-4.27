// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUI/MainGameWidget.h"

#include <string>

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "GameUI/NotificationWidget.h"
#include "Kismet/GameplayStatics.h"
#include "SlateCore/Public/Styling/SlateColor.h"
#include "Player/NHPlayerState.h"

void UMainGameWidget::UpdateHealth(float HealthPercentage)
{
	if(!Health)
	{
		UE_LOG(LogTemp, Error, TEXT("Health error"));
		return;
	}
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

void UMainGameWidget::SetWeapon1Ammo(FVector NewAmmo)
{
	auto A =  FString::FromInt(NewAmmo.X) + " / " + FString::FromInt(NewAmmo.Y); 
	Weapon1Ammo->SetText(FText::FromString(A));
	Weapon1AmmoPack->SetText(FText::AsNumber(NewAmmo.Z));
}

void UMainGameWidget::SetWeapon2Ammo(FVector NewAmmo)
{
	auto A =  FString::FromInt(NewAmmo.X) + " / " + FString::FromInt(NewAmmo.Y); 
	Weapon2Ammo->SetText(FText::FromString(A));
	Weapon2AmmoPack->SetText(FText::AsNumber(NewAmmo.Z));
}

void UMainGameWidget::SetSmallWeaponAmmo(FVector NewAmmo)
{
	auto A =  FString::FromInt(NewAmmo.X) + " / " + FString::FromInt(NewAmmo.Y); 
	SmallWeaponAmmo->SetText(FText::FromString(A));
	SmallWeaponAmmoPack->SetText(FText::AsNumber(NewAmmo.Z));
}

void UMainGameWidget::SetControlPointTeam(ENHTeams Team)
{
	if(Team == ENHTeams::None)
	{
		ControllerTeam->SetBrushTintColor(FSlateColor(FLinearColor::White)); 
	}
	else if(Team == ENHTeams::BlueTeam)
	{
		ControllerTeam->SetBrushTintColor(FSlateColor(FLinearColor::Blue)); 
	}
	else if(Team == ENHTeams::RedTeam)
	{
		ControllerTeam->SetBrushTintColor(FSlateColor(FLinearColor::Red)); 
	}
}

void UMainGameWidget::ShowTimer(int32 Time)
{
	CurrentTime = Time;
	TimerText->SetVisibility(ESlateVisibility::Visible);
	FString T = Time/60 + ":" + Time%60;
	TimerText->SetText(FText::FromString(T));
	GetOwningLocalPlayer()->GetPlayerController(GetWorld())->GetWorldTimerManager().SetTimer(TimerHandle,this, &UMainGameWidget::UpdateTime, 1);
}

void UMainGameWidget::NotifyKill(FString Message)
{
	NotificationWidget->AddMessage(Message);
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
	TimerText->SetVisibility(ESlateVisibility::Collapsed);
	SetControlPointTeam(ENHTeams::None);
}

void UMainGameWidget::UpdateTime()
{
	if(CurrentTime < 0)
	{
		CurrentTime = 0;
		TimerText->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	CurrentTime--;
	FString T = CurrentTime/60 + ":" + CurrentTime%60;
	TimerText->SetText(FText::FromString(T));
	GetOwningLocalPlayer()->GetPlayerController(GetWorld())->GetWorldTimerManager().SetTimer(TimerHandle,this, &UMainGameWidget::UpdateTime, 1);
}
