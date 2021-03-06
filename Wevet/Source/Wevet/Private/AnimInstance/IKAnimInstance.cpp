// Copyright © 2018 wevet works All Rights Reserved.


#include "AnimInstance/IKAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UIKAnimInstance::UIKAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	bEnableIK(true),
	bDebugTrace(false)
{
	IKAlpha = 1.0f;
	PelvisOffset = 0.f;
	LeftFootBoneName = TEXT("foot_l");
	RightFootBoneName = TEXT("foot_r");
	MinLimitsStanding = FVector(-10.f, -30.f, -30.f);
	MaxLimitsStanding = FVector(10.f, 30.f, 20.f);

	MinLimitsCrouching = FVector(-10.f, -30.f, -15.f);
	MaxLimitsCrouching = FVector(10.f, 30.f, 10.f);
	LeftFootOffset = FVector::ZeroVector;
	RightFootOffset = FVector::ZeroVector;
	ZOffsetInterpSpeed = 15.f;
	RotationOffsetInterpSpeed = 15.f;

	TraceLengthAboveFoot = 50.f;
	TraceLengthBelowFoot = 75.f;
}

void UIKAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	IgnoreActors.Add(GetOwningActor());
}

void UIKAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);
}

FVector UIKAnimInstance::ClampOffset(const bool WasCrouching, const float Roll, const float Pitch, const float Z) const
{
	auto Min = UKismetMathLibrary::SelectVector(MinLimitsCrouching, MinLimitsStanding, WasCrouching);
	auto Max = UKismetMathLibrary::SelectVector(MaxLimitsCrouching, MaxLimitsStanding, WasCrouching);

	auto InRoll = FMath::Clamp(Roll, Min.X, Max.X);
	auto InPitch = FMath::Clamp(Pitch, Min.Y, Max.Y);
	InPitch *= -1.f;
	auto InZ = FMath::Clamp(Z, Min.Z, Max.Z);

	return FVector(InRoll, InPitch, InZ);
}

void UIKAnimInstance::TraceOffset(const float X, const float Y, const float Z, FVector& OutStart, FVector& OutEnd)
{
	auto StartZ = Z + TraceLengthAboveFoot;
	OutStart = FVector(X, Y, StartZ);

	auto EndZ = Z - TraceLengthBelowFoot;
	OutEnd = FVector(X, Y, EndZ);
}

FVector UIKAnimInstance::InterpOffset(const FVector Current, const FVector Target) const
{
	float Delta = GetWorld()->DeltaTimeSeconds;
	FVector2D Vec2D = UKismetMathLibrary::Vector2DInterpTo(
		FVector2D(Current.X, Current.Y), 
		FVector2D(Target.X, Target.Y), 
		Delta, 
		RotationOffsetInterpSpeed);

	const float ZOffset = FMath::FInterpTo(Current.Z, Target.Z, Delta, ZOffsetInterpSpeed);

	return FVector(Vec2D.X, Vec2D.Y, ZOffset);
}

void UIKAnimInstance::UpdateIK(const bool WasCrouching)
{
	FVector RightFootOffsetTargets;
	UpdateFootIK(WasCrouching, RightFootBoneName, RightFootOffsetTargets);
	FVector LeftFootOffsetTargets;
	UpdateFootIK(WasCrouching, LeftFootBoneName, LeftFootOffsetTargets);

	LeftFootOffset = InterpOffset(LeftFootOffset, LeftFootOffsetTargets);
	RightFootOffset = InterpOffset(RightFootOffset, RightFootOffsetTargets);

	PelvisOffset = (LeftFootOffset.Z < RightFootOffset.Z) ? LeftFootOffset.Z : RightFootOffset.Z;
}

void UIKAnimInstance::UpdateFootIK(const bool WasCrouching, const FName SocketName, FVector& OutTargetPos)
{
	auto RelativeLocation = GetOwningComponent()->GetComponentLocation();
	auto BoneLocation = GetOwningComponent()->GetSocketLocation(SocketName);

	const FVector Position = FVector(BoneLocation.X, BoneLocation.Y, RelativeLocation.Z);

	FVector StartLocation;
	FVector EndLocation;
	TraceOffset(Position.X, Position.Y, Position.Z, StartLocation, EndLocation);

	const EDrawDebugTrace::Type DebugType = bDebugTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
	FHitResult HitResult;

	const bool bResult = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		StartLocation,
		EndLocation,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		true,
		IgnoreActors,
		DebugType,
		HitResult,
		true);

	const float Roll = UKismetMathLibrary::DegAtan2(HitResult.ImpactNormal.Y, HitResult.ImpactNormal.Z);
	const float Pitch = UKismetMathLibrary::DegAtan2(HitResult.ImpactNormal.X, HitResult.ImpactNormal.Z);
	const float Z = (HitResult.Location.Z - Position.Z);
	OutTargetPos = (HitResult.IsValidBlockingHit()) ? ClampOffset(WasCrouching, Roll, Pitch, Z) : FVector::ZeroVector;
}
