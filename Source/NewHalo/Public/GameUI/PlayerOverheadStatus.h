// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PlayerOverheadStatus.generated.h"

class ANHPlayerState;
class UProgressBar;
/**
 * 
 */
UCLASS()
class NEWHALO_API UPlayerOverheadStatus : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHealth(float Percent);
	
protected:

	virtual void NativeOnInitialized() override; 
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
private:

	UPROPERTY(meta=(BindWidget))
	UProgressBar* Health;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* PlayerName;

	UPROPERTY()
	ANHPlayerState* PS;
};
