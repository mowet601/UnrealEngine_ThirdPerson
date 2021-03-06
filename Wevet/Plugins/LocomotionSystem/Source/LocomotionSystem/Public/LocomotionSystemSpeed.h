// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LocomotionSystemSpeed.generated.h"

UINTERFACE(BlueprintType)
class ULocomotionSystemSpeed : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class LOCOMOTIONSYSTEM_API ILocomotionSystemSpeed
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Speed")
	void SetWalkingSpeed(const float InWalkingSpeed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Speed")
	void SetRunningSpeed(const float InRunningSpeed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Speed")
	void SetSprintingSpeed(const float InSprintingSpeed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Speed")
	void SetCrouchingSpeed(const float InCrouchingSpeed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Speed")
	void SetSwimmingSpeed(const float InSwimmingSpeed);
};
