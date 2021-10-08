// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/NewHaloCharacter.h"

#include "Equipment.h"
#include "WeaponSystem/NewHaloProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameUI/NewHaloHUD.h"
#include "GameUI/PlayerOverheadStatus.h"
#include "Interfaces/Pickable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/NHPlayerController.h"
#include "Player/NHPlayerState.h"
#include "WeaponSystem/WeaponBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ANewHaloCharacter

ANewHaloCharacter::ANewHaloCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	SetCanBeDamaged(true);
	
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(RootComponent);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
	
	/**
	 *  3rdPerson Section
	 */

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("3rdPersonMesh"));
	Mesh3P->SetOnlyOwnerSee(false);
	Mesh3P->SetupAttachment(RootComponent);
	Mesh3P->bCastDynamicShadow = true;
	Mesh3P->CastShadow = true;
	Mesh3P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh3P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	
	JumpMaxCount = 2;
	bCanSlide = true;
	
	/////
	/// Set SocketNames, Can be modified in BPs
	///

	HoldSocketName = FName("HoldSocket");
	LeftWeaponSocketName = FName("LeftWeaponSocket");
	RightWeaponSocketName = FName("RightWeaponSocket");
	PistolSocketName = FName("PistolSocket");
	/////////////////
	///
	/// Pick
	///
	///
	CurrentActionSelection = 0;
	
}

void ANewHaloCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	if(GetNetMode() == ENetMode::NM_DedicatedServer)
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Server");
	if(GetNetMode() == NM_Client)
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Client");


	auto WidgetComponent = Cast<UWidgetComponent>(GetComponentByClass(UWidgetComponent::StaticClass()));
	if(WidgetComponent)
	{
		OverHeadWidget = Cast<UPlayerOverheadStatus>(WidgetComponent->GetWidget());
		if(!OverHeadWidget)
		{
			UE_LOG(LogTemp, Error, TEXT("Cant Get OverHeadWidget"));
		}
	}

	auto PS = GetPlayerState<ANHPlayerState>();
	if(PS)
	{
		PS->RegisterLocalCharacter(this);
	}
	
	//FreeCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	OnActorBeginOverlap.AddDynamic(this, &ANewHaloCharacter::OnOverLapBegin);
	OnActorEndOverlap.AddDynamic(this, &ANewHaloCharacter::OnOverlapEnd);
	if(GetNetOwningPlayer() && GetWorld())
	{
		PC = Cast<ANHPlayerController>(GetNetOwningPlayer()->GetPlayerController(GetWorld()));
		if(!PC)
		{
			UE_LOG(LogTemp, Error, TEXT("Not PC In Character::Beginplay"))
		}
		else
		{
			PlayerHUD = PC->GetHUD<ANewHaloHUD>();
			if(!PlayerHUD)
			{
				UE_LOG(LogTemp, Error, TEXT("Not HUD In Character::Beginplay"))
			}
		}
	}
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANewHaloCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ANewHaloCharacter::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ANewHaloCharacter::OnStopFire);

	// Bind Chrouch event
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ANewHaloCharacter::OnCrouch);

	PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &ANewHaloCharacter::Slide);
	
	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ANewHaloCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANewHaloCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ANewHaloCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ANewHaloCharacter::LookUpAtRate);

///////////////////////////////
///
///
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ANewHaloCharacter::Reload);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ANewHaloCharacter::Pick);
	PlayerInputComponent->BindAxis("SwitchWeapon", this, &ANewHaloCharacter::SwitchWeapon);
}

void ANewHaloCharacter::UpdateWeaponsAmmoHUD(AWeaponBase* Weapon)
{
	if(Weapon && PlayerHUD)
	{
		auto SocketName = Weapon->GetAttachedSocketName();
		if(SocketName == RightWeaponSocketName)
		{
			PlayerHUD->SetWeapon1Ammo(Weapon->GetAmmo());
		}
		else if(SocketName == LeftWeaponSocketName)
		{
			PlayerHUD->SetWeapon2Ammo(Weapon->GetAmmo());
		}
		else if(SocketName == PistolSocketName)
		{
			PlayerHUD->SetSmallWeaponAmmo(Weapon->GetAmmo());
		}
	}
}

void ANewHaloCharacter::UpdateWeaponsIconHUD(AWeaponBase* Weapon)
{
	if(Weapon && PlayerHUD)
	{
		auto SocketName = Weapon->GetAttachedSocketName();
		if(SocketName == RightWeaponSocketName)
		{
			PlayerHUD->SetWeapon1Icon(Weapon->GetWeaponIcon());
		}
		else if(SocketName == LeftWeaponSocketName)
		{
			PlayerHUD->SetWeapon2Icon(Weapon->GetWeaponIcon());
		}
		else if(SocketName == PistolSocketName)
		{
			PlayerHUD->SetSmallWeaponIcon(Weapon->GetWeaponIcon());
		}
	}	
}

void ANewHaloCharacter::OnFire()
{
	if(CurrentWeapon)
	{
		CurrentWeapon->Fire(FP_MuzzleLocation->GetComponentLocation(), GetActorRotation());
		CurrentWeapon->SetStopFiring(false);
		UpdateWeaponsAmmoHUD(CurrentWeapon);
	}
}

void ANewHaloCharacter::OnStopFire()
{
	if(!CurrentWeapon) return;
	
	CurrentWeapon->SetStopFiring(true);
}

void ANewHaloCharacter::OnCrouch()
{
	if(!bIsCrouched)
	{
		Crouch();
		bIsCrouching =true;
	}
	else
	{
		UnCrouch();
		bIsCrouching = false;
	}
}

void ANewHaloCharacter::Slide()
{
	 if(!bCanSlide)
	 {
		 return;
	 }
	bCanSlide = false;
	bIsSliding = true;
	auto ChM = Cast<UCharacterMovementComponent>(GetMovementComponent());
	ChM->BrakingFrictionFactor = SlidingFriction;
	ChM->Velocity = GetVelocity() * SlidingSpeed;
	ChM->CrouchedHalfHeight = CapsuleHalfHeight;
	AddActorWorldOffset(FVector::UpVector * JumpFactor, true);
	Crouch();
	DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	GetWorldTimerManager().SetTimer(SlidingTimerHandle, this, &ANewHaloCharacter::StopSlide, SlidingTime, false);
}

void ANewHaloCharacter::StopSlide()
{
	UnCrouch();
	bIsSliding = false;
	auto ChM = Cast<UCharacterMovementComponent>(GetMovementComponent());
	GetWorldTimerManager().SetTimer(SlidingTimerHandle, this, &ANewHaloCharacter::ResetSlide, 1, false);
	EnableInput(GetNetOwningPlayer()->GetPlayerController(GetWorld()));
    ChM->BrakingFrictionFactor = 2;
	ChM->CrouchedHalfHeight = 40;

}

void ANewHaloCharacter::ResetSlide()
{
	bCanSlide = true;
}

void ANewHaloCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ANewHaloCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ANewHaloCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ANewHaloCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


///////////////////////////////////////////////////////////////////////////////////////////////
///
///
///
///
///
///


void ANewHaloCharacter::Reload()
{
	if(CurrentWeapon)
	{
		CurrentWeapon->Reload();
		UpdateWeaponsAmmoHUD(CurrentWeapon);
	}
}

void ANewHaloCharacter::Pick()
{
	if(OverlappingActorsList.Num() == 0)
	{
		return;
	}
	if(OverlappingActorsList[CurrentActionSelection]->IsA(AWeaponBase::StaticClass()))
	{
		auto Weapon = Cast<AWeaponBase>(OverlappingActorsList[CurrentActionSelection]);
		if(Weapon)
		{
			OverlappingActorsList.RemoveAt(CurrentActionSelection);
			Weapon->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
			Weapon->Execute_Pick(Weapon, this, 1);
			if(Weapon->GetWeaponType() == EWeaponType::Pistol)
			{
				if(Pistol)
				{
					Pistol->Execute_Drop(Pistol, 1);
				}
				Weapon->Execute_Equip(Weapon, PistolSocketName);
				if(Pistol == CurrentWeapon)
				{
					CurrentWeapon = Weapon;
					Weapon->Execute_Equip(Weapon, HoldSocketName);
				}
				Pistol = Weapon;
				UpdateWeaponsAmmoHUD(Pistol);
				UpdateWeaponsIconHUD(Pistol);

			}
			else if(!RightWeapon)
			{
				RightWeapon = Weapon;
				RightWeapon->Execute_Equip(RightWeapon, RightWeaponSocketName);
				UpdateWeaponsAmmoHUD(RightWeapon);
				UpdateWeaponsIconHUD(RightWeapon);

			}
			else if(!LeftWeapon)
			{
				LeftWeapon = Weapon;
				LeftWeapon->Execute_Equip(LeftWeapon, LeftWeaponSocketName);
				UpdateWeaponsIconHUD(LeftWeapon);
				UpdateWeaponsAmmoHUD(LeftWeapon);
			}
			else
			{
				if(!CurrentWeapon || CurrentWeapon == Pistol)
				{
					RightWeapon->Execute_Drop(RightWeapon, 1);
					RightWeapon = Weapon;
					RightWeapon->Execute_Equip(RightWeapon, RightWeaponSocketName);
					UpdateWeaponsAmmoHUD(RightWeapon);
					UpdateWeaponsIconHUD(RightWeapon);
				}
				else if(CurrentWeapon == RightWeapon)
				{
					RightWeapon->Execute_Drop(RightWeapon, 1);
					RightWeapon = Weapon;
					RightWeapon->Execute_Equip(RightWeapon, RightWeaponSocketName);
					RightWeapon->Execute_Equip(RightWeapon, HoldSocketName);
					CurrentWeapon = RightWeapon;
					UpdateWeaponsAmmoHUD(CurrentWeapon);
					UpdateWeaponsIconHUD(CurrentWeapon);
				}
				else if(CurrentWeapon == LeftWeapon)
				{
					LeftWeapon->Execute_Drop(LeftWeapon, 1);
					LeftWeapon = Weapon;
					LeftWeapon->Execute_Equip(LeftWeapon, LeftWeaponSocketName);
					LeftWeapon->Execute_Equip(LeftWeapon, HoldSocketName);
					CurrentWeapon = LeftWeapon;
					UpdateWeaponsAmmoHUD(CurrentWeapon);
					UpdateWeaponsIconHUD(CurrentWeapon);
				}
				
			}
		}
	}
	// else if(OverlappingActorsList[CurrentActionSelection]->Implements<IPickable::UClassType>())
	// {
	// 	
	// }
}

void ANewHaloCharacter::SwitchWeapon(float WeaponIndex)
{

	int32 Idx = FMath::RoundToInt(WeaponIndex);
	CurrentWeaponSelection += Idx;

	if(CurrentWeaponSelection > 3) CurrentWeaponSelection = 0;
	if(CurrentWeaponSelection < 0) CurrentWeaponSelection = 3;
	switch (CurrentWeaponSelection)
	{
		case 0:
			if(CurrentWeapon)
			{
				CurrentWeapon->Execute_Equip(CurrentWeapon, CurrentWeapon->GetAttachedSocketName());
				CurrentWeapon = nullptr;
				bIsHoldingAWeapon = false;
			}
			break;
		case 1:
			if(CurrentWeapon)
			{
				CurrentWeapon->Execute_Equip(CurrentWeapon, CurrentWeapon->GetAttachedSocketName());
			}
			if(!RightWeapon)
			{
				CurrentWeapon = nullptr;
				bIsHoldingAWeapon = false;
				break;
			}
			CurrentWeapon = RightWeapon;
			RightWeapon->Execute_Equip(RightWeapon, HoldSocketName);
			bIsHoldingAWeapon = true; 
			break;
		case 2:
			if(CurrentWeapon)
			{
				CurrentWeapon->Execute_Equip(CurrentWeapon, CurrentWeapon->GetAttachedSocketName());
			}
			if(!LeftWeapon)
			{
				CurrentWeapon = nullptr;
				bIsHoldingAWeapon = false;
				break;
			}
			CurrentWeapon = LeftWeapon;
			LeftWeapon->Execute_Equip(LeftWeapon, HoldSocketName);
			bIsHoldingAWeapon = true; 
			break;
		case 3:
			if(CurrentWeapon)
			{
				CurrentWeapon->Execute_Equip(CurrentWeapon, CurrentWeapon->GetAttachedSocketName());
			}
			if(!Pistol)
			{
				CurrentWeapon = nullptr;
				bIsHoldingAWeapon = false;
				break;
			}
			CurrentWeapon = Pistol;
			Pistol->Execute_Equip(Pistol, HoldSocketName);
			bIsHoldingAWeapon = true; 
			break;
		default:
			break;
	}
	
}

void ANewHaloCharacter::UpdateHealth(float Percent)
{
	if(OverHeadWidget)
	{
		OverHeadWidget->UpdateHealth(Percent);
	}
}

float ANewHaloCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                    AActor* DamageCauser)
{
	if(!GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return 0;
	}
	auto D = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if(DamageAmount == 0)
	{
		//TODO Show Reflection Animation with reflection sound
	}
	else
	{
		auto PS = GetPlayerState<ANHPlayerState>();
		if(PS)
		{
			//PS->ReduceHealth(DamageAmount);
			// TODO Show blood with hit sound and animation
		}
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}


void ANewHaloCharacter::OnOverLapBegin(AActor* This, AActor* Other)
{
	if(Cast<IPickable>(Other))
	{
		OverlappingActorsList.Add(Other);
	}
}

void ANewHaloCharacter::OnOverlapEnd(AActor* This, AActor* Other)
{
	if(Cast<IPickable>(Other))
	{
		OverlappingActorsList.Remove(Other);
	}
}

USkeletalMeshComponent* ANewHaloCharacter::GetMesh3P() const
{
	return Mesh3P;
}

bool ANewHaloCharacter::IsIsJumping() const
{
	return bIsJumping;
}

bool ANewHaloCharacter::IsIsDoubleJumping() const
{
	return bIsDoubleJumping;
}

bool ANewHaloCharacter::IsIsHoldingAWeapon() const
{
	return bIsHoldingAWeapon;
}

bool ANewHaloCharacter::IsIsCrouching() const
{
	return bIsCrouching;
}

bool ANewHaloCharacter::IsIsSliding() const
{
	return bIsSliding;
}

int32 ANewHaloCharacter::GetCurrentWeaponSelection() const
{
	return CurrentWeaponSelection;
}


int32 ANewHaloCharacter::GetCurrentActionSelection() const
{
	return CurrentActionSelection;
}

void ANewHaloCharacter::SetCurrentActionSelection(int32 InCurrentActionSelection)
{
	this->CurrentActionSelection = InCurrentActionSelection;
}

