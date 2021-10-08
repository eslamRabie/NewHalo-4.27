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

protected:
	virtual void BeginPlay() override;

protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);
	/** Handles stafing movement, left and right */
	void MoveRight(float Val);
	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);
	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	virtual void UpdateHealth(float Percent); 

protected:
	///
	/// Combat
	///
	///	
	/** Fires a projectile. */
	UFUNCTION(BlueprintCallable)
	virtual void OnFire();

	UFUNCTION(BlueprintCallable)
	virtual void OnStopFire();
	
	UFUNCTION(BlueprintCallable)
	virtual void OnCrouch();

	UFUNCTION(BlueprintCallable)
	virtual void Slide();

	UFUNCTION(BlueprintCallable)
	virtual void StopSlide();

	UFUNCTION(BlueprintCallable)
	virtual void ResetSlide();
	
	UFUNCTION(BlueprintCallable)
	virtual void Reload();

	UFUNCTION(BlueprintCallable)
	virtual void Pick();
	
	UFUNCTION(BlueprintCallable)
	virtual void SwitchWeapon(float WeaponIndex);
	


/////
/// Pickups
///
	UFUNCTION()
	void OnOverLapBegin(AActor* This, AActor* Other);
	UFUNCTION()
	void OnOverlapEnd(AActor* This, AActor* Other);



public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;
	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
	
	UFUNCTION(BlueprintCallable)
	FVector GetMuzzleLocation()
	{
		return FP_MuzzleLocation->GetComponentLocation();
	}

protected:

	UPROPERTY()
	ANHPlayerController* PC;

	UPROPERTY()
	ANewHaloHUD* PlayerHUD;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	//////////
	/// 3rd Person Setup
	/// 
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh3P;
public:
	USkeletalMeshComponent* GetMesh3P() const;
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Inventory)
	UInventory* InventoryComponent;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess))
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
	UPROPERTY()
	int32 CurrentActionSelection;

	// Weapon
	UPROPERTY()
	AWeaponBase* CurrentWeapon;
	UPROPERTY()
	int32 CurrentWeaponSelection;

	// Animation

	bool bIsJumping;
	bool bIsDoubleJumping;
	bool bIsCrouching;
	bool bIsSliding;
public:
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
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess), Category=Weapon)
	bool bIsHoldingAWeapon;
	
	bool bCanSlide;
	


public:
	/** Returns FirstPersonCameraComponent subobject **/
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


/**
 * The Network Section,
 *	All Network functions calls can be found here
 ***/

	
};


