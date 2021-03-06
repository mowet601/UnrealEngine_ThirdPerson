// Copyright © 2018 wevet works All Rights Reserved.

#include "Weapon/AI/AIRifle.h"
#include "Character/CharacterBase.h"
#include "Kismet/KismetMathLibrary.h"


AAIRifle::AAIRifle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponItemInfo.EquipSocketName = FName(TEXT("Weapon_Socket"));

	{
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(TEXT("/Game/Game/Blueprints/Weapon/Basic/BP_Rifle.BP_Rifle_C"));
		WeaponTemplate = FindAsset.Object;
	}

}

void AAIRifle::OnFirePressInternal()
{
	if (!CanMeleeStrike_Implementation())
	{
		return;
	}

	if (!bCanFire)
	{
		return;
	}

	if (WeaponItemInfo.CurrentAmmo <= 0)
	{
		DoReload_Implementation();
		return;
	}

	const FVector StartLocation = GetPointer()->BulletTraceRelativeLocation();
	// TargetPosition
	const FVector EndLocation = GetPointer()->BulletTraceForwardLocation();
	//const FVector ForwardLocation = GetPointer()->BulletTraceForwardLocation();
	//const FVector EndLocation = StartLocation + (ForwardLocation * WeaponItemInfo.TraceDistance);

	FHitResult HitData(ForceInit);
	FCollisionQueryParams CollisionQueryParams;
	CreateCollisionQueryParams(CollisionQueryParams);

	const bool bSuccess = GetWorld()->LineTraceSingleByChannel(
		HitData,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility,
		CollisionQueryParams);

	ISoundInstigator::Execute_ReportNoiseOther(GetPointer(), this, FireSound, DEFAULT_VOLUME, GetMuzzleTransform().GetLocation());
	GetPointer()->FireActionMontage();
	--WeaponItemInfo.CurrentAmmo;

	const FVector StartPoint = GetMuzzleTransform().GetLocation();
	const FVector EndPoint = UKismetMathLibrary::SelectVector(HitData.ImpactPoint, HitData.TraceEnd, bSuccess);
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(StartPoint, EndPoint);
	const FTransform Transform = UKismetMathLibrary::MakeTransform(StartPoint, Rotation, FVector::OneVector);
	ISoundInstigator::Execute_ReportNoiseOther(GetPointer(), this, ImpactSound, DEFAULT_VOLUME, EndPoint);

	if (bDebugTrace)
	{
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), StartLocation, EndLocation, FLinearColor::Red, 2.0f, 0.0f);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), EndLocation, 20.f, 12, FLinearColor::Red, 1.0f, 0.0f);
	}

	OnHitTriggerAction(Transform, HitData);
}
