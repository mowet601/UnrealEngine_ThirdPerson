// Copyright © 2018 wevet works All Rights Reserved.


#include "AnimInstance/PlayerAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

UPlayerAnimInstance::UPlayerAnimInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bDebugTrace = false;
	ActiveLocomotionState = ELSLocomotionState::NotMoving;
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Player = Cast<AMockCharacter>(Owner);
	InitDefaultValue();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);
	SetVariableFromOwner();
	CalculateAimOffset();

	switch (ALSMovementMode)
	{
		case ELSMovementMode::Falling:
		DoWhileFalling();
		break;

		case ELSMovementMode::Grounded:
		case ELSMovementMode::Swimming:
		DoWhileGrounded();
		DoWhileLocomotionState();
		break;

		case ELSMovementMode::Ragdoll:
		DoWhileRagdoll();
		break;
	}
}

void UPlayerAnimInstance::SetMoving()
{
	// @NOTE
	// Not Calling Parent Class
}

void UPlayerAnimInstance::SetMovementSpeed()
{
	// @NOTE
	// Not Calling Parent Class
}

#pragma region ALSInterface
void UPlayerAnimInstance::SetALSGait_Implementation(const ELSGait InLSGait)
{
	ALSGait = InLSGait;
	//UE_LOG(LogWevetClient, Log, TEXT("Gait : %s, funcName : %s"), *GETENUMSTRING("ELSGait", ALSGait), *FString(__FUNCTION__));
}

void UPlayerAnimInstance::SetALSStance_Implementation(const ELSStance InLSStance)
{
	ALSStance = InLSStance;
	OnStanceChanged();
	//UE_LOG(LogWevetClient, Log, TEXT("Stance : %s, funcName : %s"), *GETENUMSTRING("ELSStance", ALSStance), *FString(__FUNCTION__));
}

void UPlayerAnimInstance::SetALSRotationMode_Implementation(const ELSRotationMode InLSRotationMode)
{
	ALSRotationMode = InLSRotationMode;
	//UE_LOG(LogWevetClient, Log, TEXT("Rotation : %s, funcName : %s"), *GETENUMSTRING("ELSRotationMode", ALSRotationMode), *FString(__FUNCTION__));
}

void UPlayerAnimInstance::SetALSMovementMode_Implementation(const ELSMovementMode InLSMovementMode)
{
	ALSPrevMovementMode = ALSMovementMode;
	ALSMovementMode = InLSMovementMode;
	OnMovementModeChanged();
	//UE_LOG(LogWevetClient, Log, TEXT("Movement : %s, funcName : %s"), *GETENUMSTRING("ELSMovementMode", ALSMovementMode), *FString(__FUNCTION__));
}

void UPlayerAnimInstance::SetALSViewMode_Implementation(const ELSViewMode InLSViewMode)
{
	ALSViewMode = InLSViewMode;
	//UE_LOG(LogWevetClient, Log, TEXT("ViewMode : %s, funcName : %s"), *GETENUMSTRING("ELSViewMode", ALSViewMode), *FString(__FUNCTION__));
}

void UPlayerAnimInstance::SetALSIdleState_Implementation(const ELSIdleEntryState InLSIdleEntryState)
{
	ALSIdleEntryState = InLSIdleEntryState;
	//UE_LOG(LogWevetClient, Log, TEXT("Idle : %s, funcName : %s"), *GETENUMSTRING("ELSIdleEntryState", ALSIdleEntryState), *FString(__FUNCTION__));
}

void UPlayerAnimInstance::SetAiming_Implementation(const bool InAiming)
{
	bWasAiming = InAiming;
}

void UPlayerAnimInstance::SetGetup_Implementation(const bool InFaceDown)
{
	BP_ReplicatePlayMontage(InFaceDown ? GetUpFromFront : GetUpFromBack, 1.0f, 0.0f, true);
}

void UPlayerAnimInstance::SetRF_Implementation(const bool InRF)
{
	bRF = InRF;
}

void UPlayerAnimInstance::PoseSnapShot_Implementation(const FName InPoseName)
{
	SavePoseSnapshot(InPoseName);
}

void UPlayerAnimInstance::SetALSAnimNotifyTurnInPlace_Implementation(UAnimMontage* InTurnInPlaceMontage, const bool InShouldTurnInPlace, const bool InTurningInPlace, const bool InTurningRight)
{
	ActiveTurnInPlaceMontage = InTurnInPlaceMontage;
	bShouldTurnInPlace = InShouldTurnInPlace;
	bTurningInPlace = InTurningInPlace;
	bTurningRight = InTurningRight;

	if (InTurnInPlaceMontage)
	{
		UE_LOG(LogWevetClient, Log, TEXT("TurnInPlace : %s, funcName : %s"), *InTurnInPlaceMontage->GetName(), *FString(__FUNCTION__));
	}
}

void UPlayerAnimInstance::SetALSAnimNotifyPivotData_Implementation(const FPivotData InPivotData)
{
	PivotData = InPivotData;
}
#pragma endregion

#pragma region ALSMovement
void UPlayerAnimInstance::InitDefaultValue()
{
	if (!Player)
	{
		return;
	}

	ILocomotionSystemState::Execute_SetALSMovementMode(this, ILocomotionSystemPropertyGetter::Execute_GetALSMovementMode(Player));
	ILocomotionSystemState::Execute_SetALSRotationMode(this, ILocomotionSystemPropertyGetter::Execute_GetALSRotationMode(Player));
	ILocomotionSystemState::Execute_SetALSGait(this, ILocomotionSystemPropertyGetter::Execute_GetALSGait(Player));
	ILocomotionSystemState::Execute_SetALSStance(this, ILocomotionSystemPropertyGetter::Execute_GetALSStance(Player));
	ILocomotionSystemState::Execute_SetALSViewMode(this, ILocomotionSystemPropertyGetter::Execute_GetALSViewMode(Player));
	ILocomotionSystemState::Execute_SetALSIdleState(this, (ALSStance == ELSStance::Standing) ? ELSIdleEntryState::N_Idle : ELSIdleEntryState::CLF_Idle);

	ILocomotionSystemSpeed::Execute_SetWalkingSpeed(this, Player->GetWalkingSpeed());
	ILocomotionSystemSpeed::Execute_SetRunningSpeed(this, Player->GetRunningSpeed());
	ILocomotionSystemSpeed::Execute_SetSprintingSpeed(this, Player->GetSprintingSpeed());
	ILocomotionSystemSpeed::Execute_SetCrouchingSpeed(this, Player->GetCrouchingSpeed());

}

void UPlayerAnimInstance::SetVariableFromOwner()
{
	if (!Player)
	{
		return;
	}

	bWasMovementInput = ILocomotionSystemPropertyGetter::Execute_HasMovementInput(Player);
	CharacterRotation = Player->CharacterRotation;
	LastVelocityRotation = Player->LastVelocityRotation;
	LastMovementInputRotation = Player->LastMovementInputRotation;
	LookingRotation = Player->LookingRotation;
	Direction = Player->Direction;
	RotationDifference = Player->RotationDifference;
	VelocityDifference = Player->VelocityDifference;
	AimYawDelta = Player->AimYawDelta;
	AimYawRate = Player->AimYawRate;
	bWasMoving = Player->HasMoving();
	Velocity = Player->ChooseVelocity();
	bDebugTrace = Player->HasDebugTrace();
}

void UPlayerAnimInstance::DoWhileGrounded()
{
	UpdateMovementSpeed(true);
	if (bWasMoving)
	{
		WhileMoving();
	}
	else
	{
		WhileTurnInPlace();
	}
}

void UPlayerAnimInstance::DoWhileLocomotionState()
{
	switch (ActiveLocomotionState)
	{
		case ELSLocomotionState::NotMoving:
		CalculateStartPosition();
		break;
		case ELSLocomotionState::Moving:
		CalculateGroundedLeaningValues();
		break;
		case ELSLocomotionState::Pivot:
		CalcuratePivotState();
		break;
		case ELSLocomotionState::Stopping:
		break;
	}
}

void UPlayerAnimInstance::DoWhileFalling()
{
	UpdateMovementSpeed(false);
	UpdateFlailBlendAlpha();
	CalculateInAirLeaningValues();
	CalculateLandPredictionAlpha();
}

void UPlayerAnimInstance::DoWhileRagdoll()
{
	FlailRate = UKismetMathLibrary::MapRangeClamped(UKismetMathLibrary::VSize(Velocity), 0.0f, 1000.f, 0.0f, 1.25f);
}

void UPlayerAnimInstance::WhileMoving()
{
	CalculateGaitValue();

	const float Walk = 150.f;
	const float Run = 300.f;
	const float Sprint = 600.f;
	const float Crouch = 150.f;
	CalculatePlayRates(Walk, Run, Sprint, Crouch);

	const float Min = -90.f;
	const float Max = 90.f;
	const float Buffer = 5.f;
	CalculateMovementDirection(Min, Max, Buffer);
}

void UPlayerAnimInstance::WhileTurnInPlace()
{
	if (!Super::IsLocallyControlled())
		return;

	if (!Player || Player->IsPlayingRootMotion())
		return;

	if (ALSRotationMode != ELSRotationMode::LookingDirection)
		return;

	if (bWasAiming)
	{
		const float YawLimit = 60.f;
		const float PlayRate = 1.5f;
		switch (ALSStance)
		{
			case ELSStance::Standing:
			OnTurnInPlaceRespons(YawLimit, GetTurnData(), PlayRate);
			break;
			case ELSStance::Crouching:
			OnTurnInPlaceRespons(YawLimit, GetCrouchTurnData(), PlayRate);
			break;
		}
	}
	else
	{
		switch (ALSViewMode)
		{
			case ELSViewMode::FirstPerson:
			{
				const float YawLimit = 70.f;
				const float PlayRate = 1.5f;
				switch (ALSStance)
				{
					case ELSStance::Standing:
					OnTurnInPlaceRespons(YawLimit, N_Turn_90, PlayRate);
					break;
					case ELSStance::Crouching:
					OnTurnInPlaceRespons(YawLimit, GetCrouchTurnData(), PlayRate);
					break;
				}
			}
			break;

			case ELSViewMode::ThirdPerson:
			{
				if (bTurningInPlace)
				{
					return;
				}

				const float MaxCameraSpeed = 100.f;
				const float YawFirst = 60.f;
				const float YawSecond = 130.f;
				const float PlayRate = 1.5f;
				const float Delay = 0.5f;
				switch (ALSStance)
				{
					case ELSStance::Standing:
					OnTurnInPlaceDelay(MaxCameraSpeed, YawFirst, Delay, PlayRate, N_Turn_90, YawSecond, 0.0f, 1.25f, N_Turn_180);
					break;
					case ELSStance::Crouching:
					OnTurnInPlaceDelay(MaxCameraSpeed, YawFirst, Delay, 1.25f, GetCrouchTurnData(), YawSecond, 0.0f, 1.5f, GetCrouchTurnData());
					break;
				}
			}
			break;
		}
	}
}
#pragma endregion

#pragma region ALSAPI
void UPlayerAnimInstance::CalculateGaitValue()
{
	const float MoveSpeed = UKismetMathLibrary::MapRangeClamped(MovementSpeed, 0.0f, WalkingSpeed, 0.0f, 1.0f);
	const float WalkSpeed = UKismetMathLibrary::MapRangeClamped(MovementSpeed, WalkingSpeed, RunningSpeed, 1.0f, 2.0f);
	const float RunSpeed = UKismetMathLibrary::MapRangeClamped(MovementSpeed, RunningSpeed, SprintingSpeed, 2.0f, 3.0f);

	const bool bWalkedGreater = (MovementSpeed > WalkingSpeed);
	const bool bRunnedGreater = (MovementSpeed > RunningSpeed);

	float CurrentSpeed = bWalkedGreater ? WalkSpeed : MoveSpeed;
	CurrentSpeed = bRunnedGreater ? RunSpeed : CurrentSpeed;
	GaitValue = CurrentSpeed;
}

void UPlayerAnimInstance::CalculatePlayRates(const float WalkAnimSpeed, const float RunAnimSpeed, const float SprintAnimSpeed, const float CrouchAnimSpeed)
{
	const float MinRate = 0.0f;
	const float MaxRate = 1.0f;
	const float WalkValue = UKismetMathLibrary::MapRangeUnclamped(MovementSpeed, 0.0f, WalkAnimSpeed, MinRate, MaxRate);
	const float RunValue = UKismetMathLibrary::MapRangeUnclamped(MovementSpeed, 0.0f, RunAnimSpeed, MinRate, MaxRate);
	const float SprintValue = UKismetMathLibrary::MapRangeUnclamped(MovementSpeed, 0.0f, SprintAnimSpeed, MinRate, MaxRate);
	const float CrouchValue = UKismetMathLibrary::MapRangeUnclamped(MovementSpeed, 0.0f, CrouchAnimSpeed, MinRate, MaxRate);

	const float GaitWalk = UKismetMathLibrary::MapRangeClamped(GaitValue, 1.0f, 2.0f, WalkValue, RunValue);
	const float GaitRun = UKismetMathLibrary::MapRangeClamped(GaitValue, 2.0f, 3.0f, RunValue, SprintValue);
	const float Diff = (GaitValue > 2.0f) ? GaitRun : GaitWalk;
	N_PlayRate = (Diff / CapsuleComponent->GetComponentScale().Z);
	C_PlayRate = (CrouchValue / CapsuleComponent->GetComponentScale().Z);
}

void UPlayerAnimInstance::CalculateMovementDirection(const float DirectionThresholdMin, const float DirectionThresholdMax, const float Buffer)
{
	const bool A = UKismetMathLibrary::InRange_FloatFloat(Direction, (DirectionThresholdMin - Buffer), (DirectionThresholdMax + Buffer));
	const bool B = UKismetMathLibrary::InRange_FloatFloat(Direction, (DirectionThresholdMin + Buffer), (DirectionThresholdMax - Buffer));
	const bool bResult = (MovementDirection == ELSMovementDirection::Forwards) ? A : B;
	MovementDirection = bResult ? ELSMovementDirection::Forwards : ELSMovementDirection::Backwards;
}

void UPlayerAnimInstance::CalculateInAirLeaningValues()
{
	const float JumpVelocity = CharacterMovementComponent->JumpZVelocity;
	const float Value = UKismetMathLibrary::MapRangeClamped(Velocity.Z, JumpVelocity, JumpVelocity * -2.f, 1.0f, -1.0f);
	const float NormalizeValue = UKismetMathLibrary::NormalizeToRange(MovementSpeed, 0.0f, RunningSpeed);
	LeanInAir = (Value * NormalizeValue);
}

void UPlayerAnimInstance::CalculateAimOffset()
{
	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	switch (ALSRotationMode)
	{
		case ELSRotationMode::VelocityDirection:
		{
			if (bWasMovementInput)
			{
				const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LastMovementInputRotation, CharacterRotation);
				const float InterpSpeed = UKismetMathLibrary::MapRangeClamped(FMath::Abs(DeltaRot.Yaw - AimOffset.X), 0.0f, 180.f, 15.f, 5.0f);
				AimOffset = UKismetMathLibrary::Vector2DInterpTo(AimOffset, FVector2D(FMath::Clamp(DeltaRot.Yaw, -90.f, 90.f), DeltaRot.Pitch),
					DeltaSeconds,
					InterpSpeed);
			}
			else
			{
				AimOffset = UKismetMathLibrary::Vector2DInterpTo(AimOffset, FVector2D::ZeroVector, DeltaSeconds, 4.0f);
			}
		}
		break;
		case ELSRotationMode::LookingDirection:
		{
			const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LookingRotation, CharacterRotation);
			const float InterpSpeed = UKismetMathLibrary::MapRangeClamped(FMath::Abs(DeltaRot.Yaw - AimOffset.X), 0.0f, 180.f, 30.f, 5.0f);
			AimOffset = UKismetMathLibrary::Vector2DInterpTo(AimOffset, FVector2D(DeltaRot.Yaw, DeltaRot.Pitch), DeltaSeconds, InterpSpeed);
		}
		break;
	}
}

void UPlayerAnimInstance::CalculateStartPosition()
{
	if (!bWasMoving)
	{
		return;
	}

	switch (ALSStance)
	{
		case ELSStance::Standing:
		{
			if (bWasAiming)
			{
				StartPosition = (Direction > 0.0f) ? 0.187f : 1.0f;
			}
			else
			{
				StartPosition = (Direction > 0.0f) ? 0.3f : 0.867f;
			}
		}
		break;
		case ELSStance::Crouching:
		{
			StartPosition = (Direction > 0.0f) ? 0.25f : 0.5f;
		}
		break;
	}
}

void UPlayerAnimInstance::CalcuratePivotState()
{
	const float Tolerance = 45.0f;
	const bool bEqual = FMath::IsNearlyEqual(Direction, PivotData.PivotDirection, Tolerance);
	MovementDirection = (bEqual) ? PivotData.InterruptedMovementDirection : PivotData.CompletedMovementDirection;
	StartPosition = (bEqual) ? PivotData.InterruptedStartTime : PivotData.CompletedStartTime;
}

void UPlayerAnimInstance::CalculateGroundedLeaningValues()
{
	float LeanRotation;
	float LeanAcceleration;

	{
		const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
		const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LastVelocityRotation, PreviousVelocityRotation);
		DeltaVelocityDifference = (DeltaRot.Yaw / DeltaSeconds);
		PreviousVelocityRotation = LastVelocityRotation;
		const float ValueA = UKismetMathLibrary::MapRangeClamped(DeltaVelocityDifference, -200.0f, 200.0f, -1.0f, 1.0f);
		const float Speed = UKismetMathLibrary::MapRangeClamped(MovementSpeed, WalkingSpeed, RunningSpeed, 0.0f, 1.0f);
		LeanRotation = (ValueA * Speed);
	}

	{
		const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
		AccelerationDifference = (MovementSpeed - PreviousSpeed) / DeltaSeconds;
		PreviousSpeed = MovementSpeed;

		const float MaxAcceleration = CharacterMovementComponent->GetMaxAcceleration();
		const float BrakingDecelerationWalking = CharacterMovementComponent->BrakingDecelerationWalking;
		const float ValueA = UKismetMathLibrary::MapRangeClamped(FMath::Abs(AccelerationDifference), 0.0f, MaxAcceleration, 0.0f, 1.0f);
		const float ValueB = UKismetMathLibrary::MapRangeClamped(FMath::Abs(AccelerationDifference), 0.0f, BrakingDecelerationWalking, 0.0f, -1.0f);
		const float Speed = UKismetMathLibrary::MapRangeClamped(MovementSpeed, WalkingSpeed, RunningSpeed, 0.0f, 1.0f);
		LeanAcceleration = Speed * UKismetMathLibrary::SelectFloat(ValueA, ValueB, (AccelerationDifference > 0.0f));
	}

	{
		const FVector AngleAxis = UKismetMathLibrary::RotateAngleAxis(
			FVector(LeanRotation, LeanAcceleration, 0.0f),
			Direction,
			FVector(0.0f, 0.0f, -1.0f));
		LeanGrounded.X = AngleAxis.X;
		LeanGrounded.Y = AngleAxis.Y;
	}
}

void UPlayerAnimInstance::CalculateLandPredictionAlpha()
{
	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	const float InterpSpeed = 10.f;

	if (Velocity.Z > 0.0f)
	{
		LandPredictionAlpha = UKismetMathLibrary::FInterpTo(LandPredictionAlpha, 0.0f, DeltaSeconds, InterpSpeed);
		return;
	}

	if (!Player || !LandAlphaCurve)
		return;

	const FVector Location = Player->GetActorLocation();
	const float Radius = CapsuleComponent->GetScaledCapsuleRadius();
	const FVector StartLocation(Location.X, Location.Y, (Location.Z - CapsuleComponent->GetScaledCapsuleHalfHeight_WithoutHemisphere()));

	FVector EndPos = UKismetMathLibrary::Normal(FVector(Velocity.X, Velocity.Y, FMath::Clamp(Velocity.Z, -4000.f, -200.f)));
	EndPos *= UKismetMathLibrary::MapRangeClamped(Velocity.Z, 0.0f, -4000.f, 50.f, 2000.f);
	const FVector EndLocation = StartLocation + EndPos;

	FHitResult HitResult;
	const bool bResult = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		StartLocation,
		EndLocation,
		Radius,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		true,
		Player->GetIgnoreActors(),
		bDebugTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		HitResult,
		true);

	if (HitResult.bBlockingHit && (HitResult.ImpactNormal.Z >= CharacterMovementComponent->GetWalkableFloorZ()))
	{
		const float HitSpeed = 20.f;
		const float Value = UKismetMathLibrary::MapRangeClamped(HitResult.Time, 0.0f, 1.0f, 1.0f, 0.0f);
		LandPredictionAlpha = UKismetMathLibrary::FInterpTo(LandPredictionAlpha, LandAlphaCurve->GetFloatValue(Value), DeltaSeconds, HitSpeed);
	}
	else
	{
		LandPredictionAlpha = UKismetMathLibrary::FInterpTo(LandPredictionAlpha, 0.0f, DeltaSeconds, InterpSpeed);
	}
}

void UPlayerAnimInstance::OnTurnInPlaceRespons(const float AimYawLimit, const FTurnMontages TurnAnims, const float PlayRate)
{
	bShouldTurnInPlace = (FMath::Abs(AimYawDelta) > AimYawLimit);
	UAnimMontage* SelectMontage = (AimYawDelta > 0.0f) ? TurnAnims.TurnRAnim : TurnAnims.TurnLAnim;

	if (!bShouldTurnInPlace)
		return;

	bool bSuccess = false;
	if (bTurningInPlace)
	{
		if (bTurningRight)
		{
			if (!(AimYawDelta > 0.0f))
			{
				bSuccess = true;
			}
		}
		else
		{
			if (AimYawDelta > 0.0f)
			{
				bSuccess = true;
			}
		}
	}
	else
	{
		bSuccess = true;
	}

	if (bSuccess)
	{
		if (Montage_IsPlaying(SelectMontage))
			return;

		const float Value = PlayRate * UKismetMathLibrary::MapRangeClamped(FMath::Abs(AimYawRate), 120.f, 400.f, 1.0f, 2.0f);
		BP_ReplicatePlayMontage(SelectMontage, Value, 0.0f, true);
	}
}

void UPlayerAnimInstance::OnTurnInPlaceDelay(
	const float MaxCameraSpeed,
	const float AimYawLimitFirst,
	const float DelayTimeFirst,
	const float PlayRateFirst,
	const FTurnMontages TurnAnimsFirst,
	const float AimYawLimitSecond,
	const float DelayTimeSecond,
	const float PlayRateSecond,
	const FTurnMontages TurnAnimsSecond)
{
	UAnimMontage* FirstMontage = (AimYawDelta > 0.0f) ? TurnAnimsFirst.TurnRAnim : TurnAnimsFirst.TurnLAnim;
	UAnimMontage* SecondMontage = (AimYawDelta > 0.0f) ? TurnAnimsSecond.TurnRAnim : TurnAnimsSecond.TurnLAnim;
	UAnimMontage* SelectMontage = (FMath::Abs(AimYawDelta) >= AimYawLimitSecond) ? SecondMontage : FirstMontage;
	const float PlayRate = (FMath::Abs(AimYawDelta) >= AimYawLimitSecond) ? PlayRateSecond : PlayRateFirst;
	const float ClampValue = UKismetMathLibrary::MapRangeClamped(FMath::Abs(AimYawDelta), AimYawLimitFirst, AimYawLimitSecond, DelayTimeFirst, DelayTimeSecond);

	if (FMath::Abs(AimYawRate) < MaxCameraSpeed && FMath::Abs(AimYawDelta) > AimYawLimitFirst)
	{
		const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
		TurnInPlaceDelayCount = TurnInPlaceDelayCount + DeltaSeconds;

		bShouldTurnInPlace = (TurnInPlaceDelayCount > ClampValue);

		if (bShouldTurnInPlace)
		{
			if (!Montage_IsPlaying(SelectMontage))
			{
				BP_ReplicatePlayMontage(SelectMontage, PlayRate, 0.0f, true);
			}
		}
	}
	else
	{
		TurnInPlaceDelayCount = 0.0f;
	}
}

void UPlayerAnimInstance::OnStanceChanged()
{
	if (bTurningInPlace)
	{
		const float BlendOut = 0.3f;
		Montage_Stop(BlendOut, ActiveTurnInPlaceMontage ? ActiveTurnInPlaceMontage : nullptr);
	}
}

void UPlayerAnimInstance::OnMovementModeChanged()
{
	switch (ALSMovementMode)
	{
		case ELSMovementMode::Falling:
		N_PlayRate = 0.0f;
		C_PlayRate = 0.0f;
		bShouldTurnInPlace = false;
		break;
		case ELSMovementMode::Ragdoll:
		const float BlendOut = 0.2f;
		Montage_Stop(BlendOut, nullptr);
		break;
	}
}

void UPlayerAnimInstance::UpdateMovementSpeed(const bool bWasGround)
{
	FVector Value = Velocity;
	Value.Z = bWasGround ? Value.Z : 0.0f;
	MovementSpeed = UKismetMathLibrary::VSize(Value);
}

void UPlayerAnimInstance::UpdateFlailBlendAlpha()
{
	if (FlailAlphaCurve)
	{
		FlailBlendAlpha = FlailAlphaCurve->GetFloatValue(Velocity.Z * -1.f);
	}
}

void UPlayerAnimInstance::IdleTransition(UAnimSequenceBase* Animation, const float InPlayRate, const float InTimeToStartMontageAt)
{
	if (Animation)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Animation : %s, funcName : %s"), *Animation->GetName(), *FString(__FUNCTION__));

		const FName SlotName(TEXT("Transition"));
		const float BlendInTime = 0.2f;
		const float BlendOutTime = 0.2f;
		const float BlendOutTriggerTime = 0.0f;
		const int32 LoopCount = 1;
		PlaySlotAnimationAsDynamicMontage(Animation, SlotName, BlendInTime, BlendOutTime, InPlayRate, LoopCount, BlendOutTriggerTime, InTimeToStartMontageAt);
	}
}
#pragma endregion

void UPlayerAnimInstance::UpdateIKHandTransform()
{
	if (!bIKLeftHandEnabled || !Player)
		return;

	if (Player->HasEquipWeapon())
	{
		switch (Player->GetCurrentWeaponType())
		{
			case EWeaponItemType::Pistol:
			case EWeaponItemType::Rifle:
			case EWeaponItemType::Sniper:
			SetHandTransform(Player->GetSelectedWeapon()->GetGripTransform(), FTransform::Identity);
			break;

			case EWeaponItemType::Bomb:
			break;
			case EWeaponItemType::Knife:
			break;
		}
	}
	else
	{
		if (bIKLeftHandEnabled)
		{
			Super::SetArmTorsoIKMode(false, false);
		}
	}
}
