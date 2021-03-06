// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Animation/AnimMontage.h"
#include "LocomotionSystemStructs.h"
#include "LocomotionSystemTypes.h"
#include "LocomotionSystemNotify.generated.h"

UINTERFACE(BlueprintType)
class ULocomotionSystemNotify : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class LOCOMOTIONSYSTEM_API ILocomotionSystemNotify
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_AnimNotify")
	void SetALSAnimNotifyTurnInPlace(UAnimMontage* InTurnInPlaceMontage, const bool InShouldTurnInPlace, const bool InTurningInPlace, const bool InTurningRight);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_AnimNotify")
	void SetALSAnimNotifyPivotData(const FPivotData InPivotData);
};
