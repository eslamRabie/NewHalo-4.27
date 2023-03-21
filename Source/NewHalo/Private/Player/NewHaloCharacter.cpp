// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/NewHaloCharacter.h"

#include "Equipment.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/ProgressBar.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Game/NewHaloGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameUI/NewHaloHUD.h"
#include "GameUI/PlayerOverheadStatus.h"
#include "Interfaces/Pickable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Player/NHPlayerController.h"
#include "Player/NHPlayerState.h"
#include "WeaponSystem/WeaponBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ANewHaloCharacter

ANewHaloCharacter::ANewHaloCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);


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

void ANewHaloCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ANewHaloCharacter::UpdateOverHeadWidget()
{
	if(!PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant Get PS in %s"), *GetName());
		PS = GetPlayerState<ANHPlayerState>();
	}
	if(PS && OverHeadWidget)
	{
		OverHeadWidget->UpdateHealth(PS->GetHealthPercent());
		if(OverHeadWidget->PlayerNameString != PS->GetPlayerName())
		{
			OverHeadWidget->PlayerNameString = PS->GetPlayerName();
			OverHeadWidget->PlayerName->SetText(FText::FromString(OverHeadWidget->PlayerNameString));
		}
		if(OverHeadWidget->PlayerTeam != PS->GetPlayerTeam())
		{
			OverHeadWidget->PlayerTeam = PS->GetPlayerTeam();
			if (OverHeadWidget->PlayerTeam == ENHTeams::BlueTeam)
			{
				OverHeadWidget->Health->SetFillColorAndOpacity(FLinearColor::Blue);
			}
			else if (OverHeadWidget->PlayerTeam == ENHTeams::RedTeam)
			{
				OverHeadWidget->Health->SetFillColorAndOpacity(FLinearColor::Red);
			}
			else
			{
				OverHeadWidget->Health->SetFillColorAndOpacity(FLinearColor::Gray);
			}
		}
		
	}
}

void ANewHaloCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// This actually takes care of replicating the Variable
	DOREPLIFETIME(ANewHaloCharacter, OverHeadWidget);
}

void ANewHaloCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		OnActorBeginOverlap.AddDynamic(this, &ANewHaloCharacter::OnOverLapBegin);
		OnActorEndOverlap.AddDynamic(this, &ANewHaloCharacter::OnOverlapEnd);
	}
	
	
	auto WidgetComponent = Cast<UWidgetComponent>(GetComponentByClass(UWidgetComponent::StaticClass()));
	if (WidgetComponent)
	{
		OverHeadWidget = Cast<UPlayerOverheadStatus>(WidgetComponent->GetWidget());
		if (!OverHeadWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cant Get OverHeadWidget in %s"), *GetName());
		}
	}

	TryGetPSAndPC();
	GetWorldTimerManager().SetTimer(PSTimerHandle, this, &ANewHaloCharacter::UpdateOverHeadWidget, 1, false, 0);
}



///////////////////// Movement
///

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


//////////////////////////////////////////////////////////////////////////
// Input

void ANewHaloCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ANewHaloCharacter::Server_Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ANewHaloCharacter::Server_StopFire);

	// Bind Chrouch event
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ANewHaloCharacter::Server_Crouch);

	PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &ANewHaloCharacter::Server_Slide);

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
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ANewHaloCharacter::Server_Reload);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ANewHaloCharacter::Server_Pick);
	PlayerInputComponent->BindAxis("SwitchWeapon", this, &ANewHaloCharacter::Server_SwitchWeapon);
}

void ANewHaloCharacter::ServerTestRPC_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("********************************* Server RPC TEST *******************************"));
	ClientTestRPC();
	MulticastTestRPC();
}

void ANewHaloCharacter::ClientTestRPC_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("********************************* Client RPC TEST *******************************"));
}

void ANewHaloCharacter::MulticastTestRPC_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("********************************* Multicast RPC TEST *******************************"));
}


// UI

void ANewHaloCharacter::UpdateWeaponsAmmoHUD(AWeaponBase* Weapon)
{
	if (Weapon && PlayerHUD)
	{
		auto SocketName = Weapon->GetAttachedSocketName();
		if (SocketName == RightWeaponSocketName)
		{
			PlayerHUD->SetWeapon1Ammo(Weapon->GetAmmo());
		}
		else if (SocketName == LeftWeaponSocketName)
		{
			PlayerHUD->SetWeapon2Ammo(Weapon->GetAmmo());
		}
		else if (SocketName == PistolSocketName)
		{
			PlayerHUD->SetSmallWeaponAmmo(Weapon->GetAmmo());
		}
	}
}

void ANewHaloCharacter::UpdateWeaponsIconHUD(AWeaponBase* Weapon)
{
	if (Weapon && PlayerHUD)
	{
		auto SocketName = Weapon->GetAttachedSocketName();
		if (SocketName == RightWeaponSocketName)
		{
			PlayerHUD->SetWeapon1Icon(Weapon->GetWeaponIcon());
		}
		else if (SocketName == LeftWeaponSocketName)
		{
			PlayerHUD->SetWeapon2Icon(Weapon->GetWeaponIcon());
		}
		else if (SocketName == PistolSocketName)
		{
			PlayerHUD->SetSmallWeaponIcon(Weapon->GetWeaponIcon());
		}
	}
}

// Clean

void ANewHaloCharacter::Server_Fire_Implementation()
{
	if (CurrentWeapon)
	{
		Multicast_OnFire();
		Client_OnFire();
	}
}

void ANewHaloCharacter::Client_OnFire_Implementation()
{
	UpdateWeaponsAmmoHUD(CurrentWeapon);
}

void ANewHaloCharacter::Multicast_OnFire_Implementation()
{
	CurrentWeapon->Fire(FP_MuzzleLocation->GetComponentLocation(), GetActorRotation());
	CurrentWeapon->SetStopFiring(false);
}

void ANewHaloCharacter::Server_StopFire_Implementation()
{
	if (!CurrentWeapon)
	{
		return;
	}
	Multicast_OnStopFire();
}

void ANewHaloCharacter::Client_OnStopFire_Implementation()
{
}

void ANewHaloCharacter::Multicast_OnStopFire_Implementation()
{
	CurrentWeapon->SetStopFiring(true);
}

void ANewHaloCharacter::Server_Crouch_Implementation()
{
	Multicast_OnCrouch();
}

void ANewHaloCharacter::Client_OnCrouch_Implementation()
{
}

void ANewHaloCharacter::Multicast_OnCrouch_Implementation()
{
	if (!bIsCrouched)
	{
		Crouch();
		bIsCrouching = true;
	}
	else
	{
		UnCrouch();
		bIsCrouching = false;
	}
}

void ANewHaloCharacter::Server_Slide_Implementation()
{
	if (!bCanSlide)
	{
		return;
	}
	Multicast_OnSlide();
	GetWorldTimerManager().SetTimer(SlidingTimerHandle, this, &ANewHaloCharacter::Server_StopSlide, SlidingTime, false);
}

void ANewHaloCharacter::Client_OnSlide_Implementation()
{
}

void ANewHaloCharacter::Multicast_OnSlide_Implementation()
{
	bCanSlide = false;
	bIsSliding = true;
	auto ChM = Cast<UCharacterMovementComponent>(GetMovementComponent());
	ChM->BrakingFrictionFactor = SlidingFriction;
	ChM->Velocity = GetVelocity() * SlidingSpeed;
	ChM->CrouchedHalfHeight = CapsuleHalfHeight;
	AddActorWorldOffset(FVector::UpVector * JumpFactor, true);
	Crouch();
	DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void ANewHaloCharacter::Server_StopSlide_Implementation()
{
	Multicast_OnStopSlide();
	GetWorldTimerManager().SetTimer(SlidingTimerHandle, this, &ANewHaloCharacter::Server_ResetSlide, 1, false);
}

void ANewHaloCharacter::Client_OnStopSlide_Implementation()
{
}

void ANewHaloCharacter::Multicast_OnStopSlide_Implementation()
{
	UnCrouch();
	bIsSliding = false;
	auto ChM = Cast<UCharacterMovementComponent>(GetMovementComponent());
	EnableInput(GetNetOwningPlayer()->GetPlayerController(GetWorld()));
	ChM->BrakingFrictionFactor = 2;
	ChM->CrouchedHalfHeight = 40;
}

void ANewHaloCharacter::Server_ResetSlide_Implementation()
{
	Multicast_OnResetSlide();
}

void ANewHaloCharacter::Client_OnResetSlide_Implementation()
{
}

void ANewHaloCharacter::Multicast_OnResetSlide_Implementation()
{
	bCanSlide = true;
}

void ANewHaloCharacter::Server_Reload_Implementation()
{
	if (CurrentWeapon)
	{
		Multicast_OnReload();
		Client_OnReload();
	}
}

void ANewHaloCharacter::Client_OnReload_Implementation()
{
	UpdateWeaponsAmmoHUD(CurrentWeapon);
}

void ANewHaloCharacter::Multicast_OnReload_Implementation()
{
	CurrentWeapon->Reload();
}

void ANewHaloCharacter::Server_Pick_Implementation()
{
	if (OverlappingActorsList.Num() == 0)
	{
		return;
	}
	auto ActionActor = OverlappingActorsList[CurrentActionSelection];
	if(ActionActor->IsA(AWeaponBase::StaticClass()))
	{
		Multicast_OnPick(ActionActor);
		OverlappingActorsList.RemoveAt(CurrentActionSelection);
	}

}

void ANewHaloCharacter::Client_OnPick_Implementation(AActor* ActionActor)
{
}

void ANewHaloCharacter::Multicast_OnPick_Implementation(AActor* ACtionActor)
{
	if (ACtionActor->IsA(AWeaponBase::StaticClass()))
	{
		auto Weapon = Cast<AWeaponBase>(ACtionActor);
		if (Weapon)
		{
			Weapon->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
			Weapon->Execute_Pick(Weapon, this, 1);
			if (Weapon->GetWeaponType() == EWeaponType::Pistol)
			{
				if (Pistol)
				{
					Pistol->Execute_Drop(Pistol, 1);
				}
				Weapon->Execute_Equip(Weapon, PistolSocketName);
				if (Pistol == CurrentWeapon)
				{
					CurrentWeapon = Weapon;
					Weapon->Execute_Equip(Weapon, HoldSocketName);
				}
				Pistol = Weapon;
				if(HasLocalNetOwner())
				{
					UpdateWeaponsAmmoHUD(Pistol);
					UpdateWeaponsIconHUD(Pistol);
				}
			}
			else if (!RightWeapon)
			{
				RightWeapon = Weapon;
				RightWeapon->Execute_Equip(RightWeapon, RightWeaponSocketName);
				if(HasLocalNetOwner())
				{
					UpdateWeaponsAmmoHUD(RightWeapon);
					UpdateWeaponsIconHUD(RightWeapon);
				}
			}
			else if (!LeftWeapon)
			{
				LeftWeapon = Weapon;
				LeftWeapon->Execute_Equip(LeftWeapon, LeftWeaponSocketName);
				if(HasLocalNetOwner())
				{
					UpdateWeaponsIconHUD(LeftWeapon);
					UpdateWeaponsAmmoHUD(LeftWeapon);
				}
			}
			else
			{
				if (!CurrentWeapon || CurrentWeapon == Pistol)
				{
					RightWeapon->Execute_Drop(RightWeapon, 1);
					RightWeapon = Weapon;
					RightWeapon->Execute_Equip(RightWeapon, RightWeaponSocketName);
					if(HasLocalNetOwner())
					{
						UpdateWeaponsAmmoHUD(RightWeapon);
						UpdateWeaponsIconHUD(RightWeapon);
					}
				}
				else if (CurrentWeapon == RightWeapon)
				{
					RightWeapon->Execute_Drop(RightWeapon, 1);
					RightWeapon = Weapon;
					RightWeapon->Execute_Equip(RightWeapon, RightWeaponSocketName);
					RightWeapon->Execute_Equip(RightWeapon, HoldSocketName);
					CurrentWeapon = RightWeapon;
					if(HasLocalNetOwner())
					{
						UpdateWeaponsAmmoHUD(CurrentWeapon);
						UpdateWeaponsIconHUD(CurrentWeapon);
					}
				}
				else if (CurrentWeapon == LeftWeapon)
				{
					LeftWeapon->Execute_Drop(LeftWeapon, 1);
					LeftWeapon = Weapon;
					LeftWeapon->Execute_Equip(LeftWeapon, LeftWeaponSocketName);
					LeftWeapon->Execute_Equip(LeftWeapon, HoldSocketName);
					CurrentWeapon = LeftWeapon;
					if(HasLocalNetOwner())
					{
						UpdateWeaponsAmmoHUD(CurrentWeapon);
						UpdateWeaponsIconHUD(CurrentWeapon);
					}
				}
			}
		}
	}
	// else if(OverlappingActorsList[CurrentActionSelection]->Implements<IPickable::UClassType>())
	// {
	// 	
	// }	
}

void ANewHaloCharacter::Server_SwitchWeapon_Implementation(float WeaponIndex)
{
	Multicast_OnSwitchWeapon(WeaponIndex);
}

void ANewHaloCharacter::Client_OnSwitchWeapon_Implementation(float WeaponIndex)
{
}

void ANewHaloCharacter::Multicast_OnSwitchWeapon_Implementation(float WeaponIndex)
{
	int32 Idx = FMath::RoundToInt(WeaponIndex);
	CurrentWeaponSelection += Idx;

	CurrentWeaponSelection > 3? CurrentWeaponSelection = 0 : CurrentWeaponSelection < 0? CurrentWeaponSelection = 3: CurrentWeaponSelection;
	
	// if (CurrentWeaponSelection > 3)
	// {
	// 	CurrentWeaponSelection = 0;
	// }
	// if (CurrentWeaponSelection < 0)
	// {
	// 	CurrentWeaponSelection = 3;
	// }
	
	switch (CurrentWeaponSelection)
	{
	case 0:
		if (CurrentWeapon)
		{
			CurrentWeapon->Execute_Equip(CurrentWeapon, CurrentWeapon->GetAttachedSocketName());
			CurrentWeapon = nullptr;
			bIsHoldingAWeapon = false;
		}
		break;
	case 1:
		if (CurrentWeapon)
		{
			CurrentWeapon->Execute_Equip(CurrentWeapon, CurrentWeapon->GetAttachedSocketName());
		}
		if (!RightWeapon)
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
		if (CurrentWeapon)
		{
			CurrentWeapon->Execute_Equip(CurrentWeapon, CurrentWeapon->GetAttachedSocketName());
		}
		if (!LeftWeapon)
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
		if (CurrentWeapon)
		{
			CurrentWeapon->Execute_Equip(CurrentWeapon, CurrentWeapon->GetAttachedSocketName());
		}
		if (!Pistol)
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




// Fighting


void ANewHaloCharacter::ApplyDamage_Implementation(float Damage, ANHPlayerController* Player, AActor* DamageCauser)
{
	if (!GetWorld() || !GetWorld()->GetAuthGameMode())
	{
		return;
	}
	if (Damage == 0)
	{
		// the current player is in the same team as the shooting player
		//TODO Show Reflection Animation with reflection sound
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("Character Apply Damage %f, in: %s"), Damage, *GetName())
		// todo play damage vfx or or animation based on DamageCauser
		// auto GM = GetWorld()->GetAuthGameMode<ANewHaloGameMode>();
		// if (!GM)
		// {
		// 	UE_LOG(LogTemp, Error, TEXT("Cant Get GM in: ANewHaloCharacter::ApplyDamage_Implementation, in: %s"),
		// 		*GetName())
		// 	return;
		// }
		// if (!PC)
		// {
		// 	UE_LOG(LogTemp, Error, TEXT("Cant Get PC in: ANewHaloCharacter::ApplyDamage_Implementation, in: %s"),
		// 		*GetName())
		// 	return;
		// }
	//	GM->ApplyDamageToPlayer(Player, PC, Damage);
	}
}

void ANewHaloCharacter::TryGetPSAndPC()
{
	
	PC = GetController<ANHPlayerController>();
	if (!PC)
	{
		GetWorldTimerManager().SetTimer(PCTimerHandle, this, &ANewHaloCharacter::TryGetPSAndPC, 1.f, false, 0.0f);
		UE_LOG(LogTemp, Warning,TEXT("Try getting PC in ANewHaloCharacter::TryGetPSAndPC in %s"), *GetName());
		return;
	}
	else if(HasLocalNetOwner())
	{	PlayerHUD = PC->GetHUD<ANewHaloHUD>();
		if (!PlayerHUD)
		{
			GetWorldTimerManager().SetTimer(PCTimerHandle, this, &ANewHaloCharacter::TryGetPSAndPC, 1.f, false, 0.0f);
			return;
		}
	}
	GetWorldTimerManager().ClearTimer(PCTimerHandle);
}


////////////////////////////


void ANewHaloCharacter::OnOverLapBegin(AActor* This, AActor* Other)
{
	if (UKismetSystemLibrary::DoesImplementInterface(Other, UPickable::StaticClass()))
	{
		OverlappingActorsList.Add(Other);
	}
}

void ANewHaloCharacter::OnOverlapEnd(AActor* This, AActor* Other)
{
	if (UKismetSystemLibrary::DoesImplementInterface(Other, UPickable::StaticClass()))
	{
		OverlappingActorsList.Remove(Other);
	}
}




FVector ANewHaloCharacter::GetMuzzleLocation()
{
	return FP_MuzzleLocation->GetComponentLocation();
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
