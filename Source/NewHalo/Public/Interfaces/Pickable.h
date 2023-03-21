// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pickable.generated.h"

class ANewHaloCharacter;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickable : public UInterface
{
	GENERATED_BODY()
};

/**
* 
*/
class NEWHALO_API IPickable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Pick(ANewHaloCharacter* PickingPlayer, int Amount = 1);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Drop(int Amount = 1);
};
