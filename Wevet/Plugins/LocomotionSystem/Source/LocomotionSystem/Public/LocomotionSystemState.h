// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LocomotionSystemTypes.h"
#include "LocomotionSystemState.generated.h"

UINTERFACE(BlueprintType)
class ULocomotionSystemState : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

// @NOTE
// AnimInstanceClassで呼ばれる
class LOCOMOTIONSYSTEM_API ILocomotionSystemState
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_State")
	void SetALSGait(const ELSGait InLSGait);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_State")
	void SetALSStance(const ELSStance InLSStance);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_State")
	void SetALSRotationMode(const ELSRotationMode InLSRotationMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_State")
	void SetALSMovementMode(const ELSMovementMode InLSMovementMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_State")
	void SetALSViewMode(const ELSViewMode InLSViewMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_State")
	void SetALSIdleState(const ELSIdleEntryState InLSIdleEntryState);
};
