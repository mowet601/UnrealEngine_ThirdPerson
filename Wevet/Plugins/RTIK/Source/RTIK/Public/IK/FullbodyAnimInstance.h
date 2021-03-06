// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HumanoidIK.h"
#include "IKTypes.h"
#include "FullbodyAnimInstance.generated.h"

/**
 * 
 */
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class RTIK_API UFullbodyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFullbodyAnimInstance(const FObjectInitializer& ObjectInitializer);
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	float MaxPelvisAdjustHeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	float LeftArmIKAlpha;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	float RightArmIKAlpha;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	float IKTargetInterpolationSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	EHumanoidLegIKMode LeftFootIKMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	EHumanoidLegIKMode RightFootIKMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	EHumanoidArmTorsoIKMode ArmTorsoIKMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	FTransform IKLeftLegTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	FTransform IKRightLegTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	FTransform IKLeftHandTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	FTransform IKRightHandTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	FTransform IKLeftHandTargetInterpolated;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	FTransform IKRightHandTargetInterpolated;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	struct FIKBone PelvisBone;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	struct FRangeLimitedIKChain RightArmChain;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	struct FRangeLimitedIKChain LeftArmChain;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	struct FHumanoidLegChain RightLeg;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	struct FHumanoidLegChain LeftLeg;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	class UHumanoidLegChain_Wrapper* LeftLegWrapper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	class UHumanoidLegChain_Wrapper* RightLegWrapper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	class UIKBoneWrapper* PelvisBoneWrapper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	class UHumanoidIKTraceData_Wrapper* LeftTraceDataWrapper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	class UHumanoidIKTraceData_Wrapper* RightTraceDataWrapper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	class URangeLimitedIKChainWrapper* LeftArmChainWrapper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	class URangeLimitedIKChainWrapper* RightArmChainWrapper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	bool bIKLeftHandEnabled;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IKVariables")
	bool bIKRightHandEnabled;

protected:
	UFUNCTION(BlueprintCallable)
	void SetLegTransform(const FTransform InLeftFootTransform, const FTransform InRightFootTransform);

	UFUNCTION(BlueprintCallable)
	void SetHandTransform(const FTransform InLeftHandTransform, const FTransform InRightHandTransform);

	void SetIKHandTarget(const FTransform InTransform, FTransform& OutIKTarget, FTransform& OutIKTargetInterpolated);
	void SetArmTorsoIKMode();

public:
	UFUNCTION(BlueprintCallable)
	void SetArmTorsoIKMode(const bool InIKLeftHandOntoTarget, const bool InIKRightHandOntoTarget);

	UFUNCTION(BlueprintCallable)
	void SetLegOntoTarget(const bool InIKLeftFootEnable, const bool InIKRightFootEnable);

};
