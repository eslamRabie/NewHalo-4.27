// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NewHaloCharacter.generated.h"

class UPlayerOverheadStatus;
class ANewHaloHUD;
class UInventory;
class ANHPlayerState;
class AWeaponBase;
class ANHPlayerController;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ANewHaloCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ANewHaloCharacter();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;
	void MoveForward(float Val);
	void MoveRight(float Val);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	UFUNCTION(Server, Reliable)
	void ServerTestRPC();

	UFUNCTION(Client, Reliable)
	void ClientTestRPC();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastTestRPC();
	
	
public:
	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void ApplyDamage(float Damage, ANHPlayerController* Player, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	FVector GetMuzzleLocation();

	UFUNCTION(BlueprintCallable)
	bool IsIsJumping() const;

	UFUNCTION(BlueprintCallable)
	bool IsIsDoubleJumping() const;

	UFUNCTION(BlueprintCallable)
	bool IsIsHoldingAWeapon() const;

	UFUNCTION(BlueprintCallable)
	bool IsIsCrouching() const;

	UFUNCTION(BlueprintCallable)
	bool IsIsSliding() const;

	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentWeaponSelection() const;

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentActionSelection() const;

	UFUNCTION(BlueprintCallable)
	void SetCurrentActionSelection(int32 InCurrentActionSelection);

	UFUNCTION()
	void UpdateWeaponsAmmoHUD(AWeaponBase* Weapon);

	UFUNCTION()
	void UpdateWeaponsIconHUD(AWeaponBase* Weapon);

protected:


	/////
	/// Pickups
	///
	UFUNCTION()
	void OnOverLapBegin(AActor* This, AActor* Other);
	UFUNCTION()
	void OnOverlapEnd(AActor* This, AActor* Other);
	UFUNCTION()
	void TryGetPSAndPC();
	
	void UpdateOverHeadWidget();

	
	

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:
	UPROPERTY()
	ANHPlayerController* PC;
	UPROPERTY()
	ANewHaloHUD* PlayerHUD;
	UPROPERTY()
	ANHPlayerState* PS;

	FTimerHandle PCTimerHandle;
	FTimerHandle PSTimerHandle;

	
	
	
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess))
	UCameraComponent* FirstPersonCameraComponent;
	

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Inventory)
	UInventory* InventoryComponent;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess), Replicated)
	UPlayerOverheadStatus* OverHeadWidget;

	///
	/// Weapons
	///
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	AWeaponBase* LeftWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	AWeaponBase* RightWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	AWeaponBase* Pistol;
	///
	/// Sockets Names
	///
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FName HoldSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FName LeftWeaponSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FName RightWeaponSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FName PistolSocketName;


	// pickups

	UPROPERTY()
	TArray<AActor*> OverlappingActorsList;

	
	// Weapon
	UPROPERTY()
	AWeaponBase* CurrentWeapon;
	UPROPERTY()
	int32 CurrentWeaponSelection;
	UPROPERTY()
	int32 CurrentActionSelection;

	// Animation
	bool bCanSlide;
	bool bIsJumping;
	bool bIsDoubleJumping;
	bool bIsCrouching;
	bool bIsSliding;

private:
	FTimerHandle SlidingTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Sliding)
	float SlidingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Sliding)
	float SlidingTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Sliding)
	float SlidingFriction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Sliding)
	float CapsuleHalfHeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Jumping)
	float JumpFactor;

	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Weapon)
	bool bIsHoldingAWeapon;

	//FTimerHandle PCTimerHandle;

protected:
	// Server Callbacks
	// Clean

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_Fire();
	UFUNCTION(BlueprintCallable, Client, Reliable)
	virtual void Client_OnFire();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multicast_OnFire();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_StopFire();
	UFUNCTION(BlueprintCallable, Client, Reliable)
	virtual void Client_OnStopFire();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multicast_OnStopFire();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_Crouch();
	UFUNCTION(BlueprintCallable, Client, Reliable)
	virtual void Client_OnCrouch();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multicast_OnCrouch();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_Slide();
	UFUNCTION(BlueprintCallable, Client, Reliable)
	virtual void Client_OnSlide();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multicast_OnSlide();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_StopSlide();
	UFUNCTION(BlueprintCallable, Client, Reliable)
	virtual void Client_OnStopSlide();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multicast_OnStopSlide();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_ResetSlide();
	UFUNCTION(BlueprintCallable, Client, Reliable)
	virtual void Client_OnResetSlide();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multicast_OnResetSlide();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_Reload();
	UFUNCTION(BlueprintCallable, Client, Reliable)
	virtual void Client_OnReload();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multicast_OnReload();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_Pick();
	UFUNCTION(BlueprintCallable, Client, Reliable)
	virtual void Client_OnPick(AActor* ActionActor);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multicast_OnPick(AActor* ActionActor);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Server_SwitchWeapon(float WeaponIndex);
	UFUNCTION(BlueprintCallable, Client, Reliable)
	virtual void Client_OnSwitchWeapon(float WeaponIndex);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Multicast_OnSwitchWeapon(float WeaponIndex);
	
};




