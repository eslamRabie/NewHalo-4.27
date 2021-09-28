// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Player/NHPlayerState.h"
#include "NewHaloHUD.generated.h"

class UMainGameWidget;
UCLASS()
class ANewHaloHUD : public AHUD
{
	GENERATED_BODY()

public:
	ANewHaloHUD();
	
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
	virtual void BeginPlay() override;

private:
	void CreateMainGameWidget();

private:
	/** Crosshair asset pointer */
	UPROPERTY()
	class UTexture2D* CrosshairTex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category=NHHUD)
	TSubclassOf<UMainGameWidget> MainGameWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=NHHUD)
	UMainGameWidget* MainGameWidget;
};

