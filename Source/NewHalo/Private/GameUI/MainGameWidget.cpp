// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUI/MainGameWidget.h"


#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Game/NHControlGameState.h"
#include "GameUI/NotificationWidget.h"
#include "Kismet/GameplayStatics.h"
#include "SlateCore/Public/Styling/SlateColor.h"
#include "Player/NHPlayerState.h"



void UMainGameWidget::UpdateHealth(float HealthPercentage)
{
	if (!Health)
	{
		UE_LOG(LogTemp, Warning, TEXT("Health error in %s"), *GetName());
		return;
	}
	Health->SetPercent(HealthPercentage);
}

void UMainGameWidget::SetWinning(ENHTeams WinnerTeamm)
{
	FString Message = "No Data";
	if(PS)
	{
		if(WinnerTeamm == PS->GetPlayerTeam())
		{
			Message = "Your Team Won!";
		}
		else
		{
			Message = "Your Team Lose!";
		}
	}
	EndGameText->SetText(FText::FromString(Message));
}

void UMainGameWidget::SetInventoryItemIcon(UTexture2D* Icon, int32 SlotIndex)
{
	auto Wid = Cast<UImage>(Inventory->GetChildAt(SlotIndex));
	if(Wid)
		Wid->SetBrushFromTexture(Icon);
}

void UMainGameWidget::SetInventoryItemCount(int32 Count, int32 SlotIndex)
{
}

void UMainGameWidget::SetWeapon1Icon(UTexture2D* Icon)
{
	if (!Icon)
	{
		return;
	}
	Weapon1Icon->SetBrushFromTexture(Icon);
}

void UMainGameWidget::SetWeapon2Icon(UTexture2D* Icon)
{
	if (!Icon)
	{
		return;
	}
	Weapon2Icon->SetBrushFromTexture(Icon);
}

void UMainGameWidget::SetSmallWeaponIcon(UTexture2D* Icon)
{
	if (!Icon)
	{
		return;
	}
	SmallWeaponIcon->SetBrushFromTexture(Icon);
}

void UMainGameWidget::SetWeapon1Ammo(FVector NewAmmo)
{
	auto A = FString::FromInt(NewAmmo.X) + " / " + FString::FromInt(NewAmmo.Y);
	Weapon1Ammo->SetText(FText::FromString(A));
	Weapon1AmmoPack->SetText(FText::AsNumber(NewAmmo.Z));
}

void UMainGameWidget::SetWeapon2Ammo(FVector NewAmmo)
{
	auto A = FString::FromInt(NewAmmo.X) + " / " + FString::FromInt(NewAmmo.Y);
	Weapon2Ammo->SetText(FText::FromString(A));
	Weapon2AmmoPack->SetText(FText::AsNumber(NewAmmo.Z));
}

void UMainGameWidget::SetSmallWeaponAmmo(FVector NewAmmo)
{
	auto A = FString::FromInt(NewAmmo.X) + " / " + FString::FromInt(NewAmmo.Y);
	SmallWeaponAmmo->SetText(FText::FromString(A));
	SmallWeaponAmmoPack->SetText(FText::AsNumber(NewAmmo.Z));
}

void UMainGameWidget::SetControlPointTeam(ENHTeams Team)
{
	if (Team == ENHTeams::None)
	{
		ControllerTeam->SetBrushTintColor(FSlateColor(FLinearColor::Gray));
	}
	else if (Team == ENHTeams::BlueTeam)
	{
		ControllerTeam->SetBrushTintColor(FSlateColor(FLinearColor::Blue));
	}
	else if (Team == ENHTeams::RedTeam)
	{
		ControllerTeam->SetBrushTintColor(FSlateColor(FLinearColor::Red));
	}
}

void UMainGameWidget::ShowTimer(float Time)
{
	CurrentTime = Time;
	TimerText->SetVisibility(ESlateVisibility::Visible);
	FString T = FString::FromInt(FMath::RoundToInt(Time) / 60)  + ":" + FString::FromInt(FMath::RoundToInt(Time) % 60);
	TimerText->SetText(FText::FromString(T));
	GetOwningLocalPlayer()->GetPlayerController(GetWorld())->GetWorldTimerManager().SetTimer(
		TimerHandle, this, &UMainGameWidget::UpdateTime, 1);
}

void UMainGameWidget::NotifyKill(FString Message)
{
	NotificationWidget->AddMessage(Message);
}

void UMainGameWidget::UpdateKills(int32 Kills)
{
	KillsText->SetText(FText::AsNumber(Kills));
}

void UMainGameWidget::UpdateDeaths(int32 Deaths)
{
	DeathsText->SetText(FText::AsNumber(Deaths));
}

void UMainGameWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!Health || !ControllerTeam || !Weapon1Ammo || !Weapon2Ammo || !SmallWeaponAmmo || !Weapon1Icon || ! Weapon2Icon
		||
		!SmallWeaponIcon || !Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("Set vars in BP in %s"), *GetName());
		return;
	}
	TimerText->SetVisibility(ESlateVisibility::Collapsed);
	SetControlPointTeam(ENHTeams::None);
	TickWaitingTime = 0;
}

void UMainGameWidget::NativeTick(const FGeometry& MovieSceneBlends, float InDeltaTime)
{
	Super::NativeTick(MovieSceneBlends, InDeltaTime);
	if(TickWaitingTime < 10)
	{
		TickWaitingTime += InDeltaTime;
		return;
	}
	if(!GS)
	{
		GS = GetWorld()->GetGameState<ANHControlGameState>();
		UE_LOG(LogTemp, Log, TEXT("Try To Get GS in %s"), *GetName());

	}
	if(GS)
	{
		if(ControlTeam != GS->GetControlTeam())
		{
			ControlTeam = GS->GetControlTeam();
			SetControlPointTeam(ControlTeam);
		}
		SetControlPointTime(GS->GetControlPointTime());
	}
	if(!PS)
	{
		PS = GetOwningPlayerState<ANHPlayerState>();
	}
	if(PS)
	{
		UpdateDeaths(PS->GetDeaths());
		UpdateHealth(PS->GetHealthPercent());
		UpdateKills(PS->GetKills());
	}
}

void UMainGameWidget::UpdateTime()
{
	if (CurrentTime <= 0)
	{
		CurrentTime = 0;
		TimerText->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	CurrentTime -= 1;
	FString T = FString::FromInt(FMath::RoundToInt(CurrentTime) / 60) + ":" + FString::FromInt(FMath::RoundToInt(CurrentTime) % 60);
	TimerText->SetText(FText::FromString(T));
	GetOwningLocalPlayer()->GetPlayerController(GetWorld())->GetWorldTimerManager().SetTimer(
		TimerHandle, this, &UMainGameWidget::UpdateTime, 1);
}

void UMainGameWidget::SetControlPointTime(float Time)
{
	FString T = FString::FromInt(FMath::RoundToInt(Time) / 60) + ":" + FString::FromInt(FMath::RoundToInt(Time) % 60);
	ControlPointTime->SetText(FText::FromString(T));
}
