// Copyright © 2018 wevet works All Rights Reserved.

#include "AIWeaponBase.h"
#include "CharacterBase.h"
#include "AICharacterBase.h"
#include "BulletBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AAIWeaponBase::AAIWeaponBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AAIWeaponBase::OnFirePressedInternal()
{
	UWorld* const World = GetWorld();

	if (World == nullptr || 
		Owner == nullptr || 
		(IDamageInstigator::Execute_IsDeath(Owner)) || 
		(Owner->GetTarget_Implementation() == nullptr))
	{
		return;
	}

	if (Super::bEmpty || Super::bReload)
	{
		return;
	}

	if (Super::WeaponItemInfo.CurrentAmmo <= 0)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Out Of Ammos"));
		Super::OnReloading_Implementation();
		return;
	}

	const FVector StartLocation   = Owner->BulletTraceRelativeLocation();
	const FVector ForwardLocation = Owner->BulletTraceForwardLocation();
	const FVector EndLocation = StartLocation + (ForwardLocation * WeaponItemInfo.TraceDistance);

	FHitResult HitData(ForceInit);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.TraceTag = FName("");
	CollisionQueryParams.OwnerTag = FName("");
	//CollisionQueryParams.bTraceAsyncScene = false;
	CollisionQueryParams.bTraceComplex = true;
	CollisionQueryParams.bFindInitialOverlaps = false;
	CollisionQueryParams.bReturnFaceIndex = false;
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bIgnoreBlocks = false;
	CollisionQueryParams.IgnoreMask = 0;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.AddIgnoredActor(CharacterOwner.Get());

	const bool bSuccess = World->LineTraceSingleByChannel(
		HitData,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility,
		CollisionQueryParams);

	//UKismetSystemLibrary::DrawDebugLine(World, StartLocation, HitData.ImpactPoint, FLinearColor(255, 0, 0, 0), 1.0f, 1.0f);

	//if (!bSuccess)
	//{
	//	return;
	//}

	const FVector MuzzleLocation  = StartLocation;
	const FRotator MuzzleRotation = FRotator(GetMuzzleTransform().GetRotation());
	IInteractionExecuter::Execute_ReportNoiseOther(Owner, this, FireSoundAsset, DEFAULT_VOLUME, MuzzleLocation);
	Owner->FireActionMontage();
	--WeaponItemInfo.CurrentAmmo;

	IInteractionExecuter::Execute_ReportNoiseOther(Owner, this, FireImpactSoundAsset, DEFAULT_VOLUME, HitData.Location);
	const FVector StartPoint = MuzzleLocation;
	const FVector EndPoint   = UKismetMathLibrary::SelectVector(HitData.ImpactPoint, HitData.TraceEnd, bSuccess);
	const FRotator Rotation  = UKismetMathLibrary::FindLookAtRotation(StartPoint, HitData.ImpactPoint);
	FTransform Transform = UKismetMathLibrary::MakeTransform(StartPoint, Rotation, FVector::OneVector);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	ABulletBase* const Bullet = World->SpawnActor<ABulletBase>(Super::BulletsBP, Transform, SpawnParams);

#if WITH_EDITOR
	Bullet->SetFolderPath("/BulletsRoot");
#endif

	Super::TakeHitDamage(HitData);
	Super::PlayBulletEffect(World, HitData);
}

void AAIWeaponBase::SetCharacterOwner(ACharacterBase* InCharacterOwner)
{
	Super::SetCharacterOwner(InCharacterOwner);

	bool bCastSuccess = false;
	if (Super::CharacterOwner.IsValid())
	{
		if (AAICharacterBase* AICharacter = Cast<AAICharacterBase>(Super::CharacterOwner.Get()))
		{
			Owner = AICharacter;
			bCastSuccess = true;
		}
	}
	if (!bCastSuccess)
	{
		Owner = nullptr;
		UE_LOG(LogWevetClient, Error, TEXT("CastFailed : %s \n [%s]"), *GetName(), *FString(__FUNCTION__));
	}
}
