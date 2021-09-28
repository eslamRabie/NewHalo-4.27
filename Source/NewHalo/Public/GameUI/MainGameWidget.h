// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainGameWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UUniformGridPanel;
enum class ETeams;
/**
 * 
 */
UCLASS()
class NEWHALO_API UMainGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void UpdateHealth(float HealthPercentage);
	
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

	void SetWeapon1Ammo(int32 NewAmmo);
	
	void SetWeapon2Ammo(int32 NewAmmo);

	void SetSmallWeaponAmmo(int32 NewAmmo);

	/////////////////
	///
	/// Control Point
	///

	void SetControlPointTeam(ETeams Team);

protected:
	virtual void NativeOnInitialized() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* Inventory;

	UPROPERTY(meta=(BindWidget))
	UProgressBar* Health;

	UPROPERTY(meta=(BindWidget))
	UImage* ControllerTeam;
	
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

	
	
	
};
