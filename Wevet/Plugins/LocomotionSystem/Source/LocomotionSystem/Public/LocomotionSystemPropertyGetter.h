// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LocomotionSystemTypes.h"
#include "LocomotionSystemPropertyGetter.generated.h"

UINTERFACE(BlueprintType)
class ULocomotionSystemPropertyGetter : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class LOCOMOTIONSYSTEM_API ILocomotionSystemPropertyGetter
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_PropertyGetter")
	ELSMovementMode GetALSMovementMode() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_PropertyGetter")
	ELSMovementAction GetALSMovementAction() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_PropertyGetter")
	ELSGait GetALSGait() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_PropertyGetter")
	ELSStance GetALSStance() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_PropertyGetter")
	ELSViewMode GetALSViewMode() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_PropertyGetter")
	ELSRotationMode GetALSRotationMode() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_PropertyGetter")
	bool HasMovementInput() const;
};
