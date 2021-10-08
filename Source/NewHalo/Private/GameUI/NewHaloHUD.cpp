// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameUI/NewHaloHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "GameUI/MainGameWidget.h"

ANewHaloHUD::ANewHaloHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;

	///
	/// 
	///
}



void ANewHaloHUD::UpdateHealth(float HealthPercentage)
{
	MainGameWidget->UpdateHealth(HealthPercentage);
}

void ANewHaloHUD::SetInventoryItemIcon(UTexture2D* Icon, int32 SlotIndex)
{
	MainGameWidget->SetInventoryItemIcon(Icon, SlotIndex);
}

void ANewHaloHUD::SetInventoryItemCount(int32 Count, int32 SlotIndex)
{
	//todo
}

void ANewHaloHUD::SetWeapon1Icon(UTexture2D* Icon)
{
	MainGameWidget->SetWeapon1Icon(Icon);
}

void ANewHaloHUD::SetWeapon2Icon(UTexture2D* Icon)
{
	MainGameWidget->SetWeapon2Icon(Icon);
}

void ANewHaloHUD::SetSmallWeaponIcon(UTexture2D* Icon)
{
	MainGameWidget->SetSmallWeaponIcon(Icon);
}

void ANewHaloHUD::SetWeapon1Ammo(FVector NewAmmo)
{
	MainGameWidget->SetWeapon1Ammo(NewAmmo);
}

void ANewHaloHUD::SetWeapon2Ammo(FVector NewAmmo)
{
	MainGameWidget->SetWeapon2Ammo(NewAmmo);
}

void ANewHaloHUD::SetSmallWeaponAmmo(FVector NewAmmo)
{
	MainGameWidget->SetSmallWeaponAmmo(NewAmmo);
}

void ANewHaloHUD::SetControlPointTeam(ENHTeams Team)
{
	MainGameWidget->SetControlPointTeam(Team);
}

void ANewHaloHUD::SetStartGameTimer(float Time)
{
	MainGameWidget->ShowTimer(Time);
}

void ANewHaloHUD::SetEndGameTimer(float Time)
{
	MainGameWidget->ShowTimer(Time);
}

void ANewHaloHUD::ShowWinner(ENHTeams WinnerTeam)
{
	//todo
}

void ANewHaloHUD::NotifyKill(FString KillerName, FString TargetName)
{
	MainGameWidget->NotifyKill(FString(KillerName + " Killed " + TargetName));
}

void ANewHaloHUD::BeginPlay()
{
	Super::BeginPlay();
	CreateMainGameWidget();
}

void ANewHaloHUD::CreateMainGameWidget()
{
	if(!MainGameWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("MainGameWidgetClass is null in %S"), *GetName());
		return;
	}

	MainGameWidget = CreateWidget<UMainGameWidget>(GetWorld(), MainGameWidgetClass);

	if(!MainGameWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant Create MAinGameWidget in ANHHUD in %S"), *GetName());
	}
	MainGameWidget->AddToViewport();
}
