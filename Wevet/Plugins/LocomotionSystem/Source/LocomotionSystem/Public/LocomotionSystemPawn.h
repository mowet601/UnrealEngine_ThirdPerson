// Copyright 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Camera/CameraShake.h"
#include "LocomotionSystemTypes.h"
#include "LocomotionSystemPawn.generated.h"

UINTERFACE(BlueprintType)
class ULocomotionSystemPawn : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class LOCOMOTIONSYSTEM_API ILocomotionSystemPawn
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSCharacterRotation(const FRotator AddAmount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSCameraShake(TSubclassOf<class UCameraShake> InShakeClass, const float InScale);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSMovementMode(const ELSMovementMode NewALSMovementMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void OnALSMovementModeChange();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSMovementAction(const ELSMovementAction NewALSMovementAction);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void OnALSMovementActionChange();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSGait(const ELSGait NewALSGait);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void OnALSGaitChange();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSStance(const ELSStance NewALSStance);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void OnALSStanceChange();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSRotationMode(const ELSRotationMode NewALSRotationMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void OnALSRotationModeChange();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSViewMode(const ELSViewMode NewALSViewMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void OnALSViewModeChange();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void SetALSAiming(const bool NewALSAiming);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS_Pawn")
	void OnALSAimingChange();
};
