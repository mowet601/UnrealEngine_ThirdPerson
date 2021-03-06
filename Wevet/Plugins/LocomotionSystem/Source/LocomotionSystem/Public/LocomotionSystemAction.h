// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Camera/CameraShake.h"
#include "LocomotionSystemAction.generated.h"

UINTERFACE(BlueprintType)
class ULocomotionSystemAction : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class LOCOMOTIONSYSTEM_API ILocomotionSystemAction
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Action")
	void SetAiming(const bool InAiming);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Action")
	void SetGetup(const bool InFaceDown);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Action")
	void SetRF(const bool InRF);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Action")
	void PoseSnapShot(const FName InPoseName);
};
