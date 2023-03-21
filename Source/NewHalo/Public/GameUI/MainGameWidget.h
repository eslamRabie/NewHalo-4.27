// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/NHPlayerState.h"
#include "MainGameWidget.generated.h"

class ANHControlGameState;
class UNotificationWidget;
class UProgressBar;
class UTextBlock;
class UImage;
class UUniformGridPanel;
/**
 * 
 */
UCLASS()
class NEWHALO_API UMainGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHealth(float HealthPercentage);

	void SetWinning(ENHTeams WinnerTeamm);
	///////////////
	///
	///Inventory Slots
	///

	void SetInventoryItemIcon(UTexture2D* Icon, int32 SlotIndex);

	void SetInventoryItemCount(int32 Count, int32 SlotIndex);


	//////////////
	///
	/// Main Weapons Slots
	///

	void SetWeapon1Icon(UTexture2D* Icon);

	void SetWeapon2Icon(UTexture2D* Icon);

	void SetSmallWeaponIcon(UTexture2D* Icon);

	void SetWeapon1Ammo(FVector NewAmmo);

	void SetWeapon2Ammo(FVector NewAmmo);

	void SetSmallWeaponAmmo(FVector NewAmmo);

	/////////////////
	///
	/// Control Point
	///

	void SetControlPointTeam(ENHTeams Team);


	////
	/// Show Timer
	/// 
	void ShowTimer(float Time);

	/// Notifykill
	///
	void NotifyKill(FString Message);

	// Kills Deaths
	void UpdateKills(int32 Kills);

	void UpdateDeaths(int32 Deaths);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void UpdateTime();
	void SetControlPointTime(float Time);
private:
	UPROPERTY(meta=(BindWidget))
	UNotificationWidget* NotificationWidget;

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* Inventory;

	UPROPERTY(meta=(BindWidget))
	UProgressBar* Health;

	UPROPERTY(meta=(BindWidget))
	UImage* ControllerTeam;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ControlPointTime;

	UPROPERTY()
	ANHControlGameState* GS;

	ENHTeams ControlTeam;

	UPROPERTY()
	ANHPlayerState* PS;

	UPROPERTY()
	TArray<UImage*> Slots;

	UPROPERTY(meta=(BindWidget))
	UImage* Weapon1Icon;
	UPROPERTY(meta=(BindWidget))
	UImage* Weapon2Icon;
	UPROPERTY(meta=(BindWidget))
	UImage* SmallWeaponIcon;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* Weapon1Ammo;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* Weapon2Ammo;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* SmallWeaponAmmo;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* Weapon1AmmoPack;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* Weapon2AmmoPack;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* SmallWeaponAmmoPack;

	UPROPERTY(meta=(bindWidget))
	UTextBlock* TimerText;

	UPROPERTY(meta=(bindWidget))
	UTextBlock* EndGameText;

	UPROPERTY(meta=(bindWidget))
	UTextBlock* KillsText;
	UPROPERTY(meta=(bindWidget))
	UTextBlock* DeathsText;

	FTimerHandle TimerHandle;

	float CurrentTime;

	float TickWaitingTime;
};
