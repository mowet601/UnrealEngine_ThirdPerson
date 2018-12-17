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

}

void AAIWeaponBase::OnFirePressedInternal()
{
	UWorld* const World = GetWorld();

	if (World == nullptr || Owner == nullptr || (Owner && Owner->IsDeath_Implementation()) || (Owner->GetTargetCharacter() == nullptr))
	{
		return;
	}

	if (Super::bEmpty || Super::bReload)
	{
		return;
	}

	if (Super::WeaponItemInfo.CurrentAmmo <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Out Of Ammos"));
		Super::OnReloading_Implementation();
		return;
	}

	const float ForwardOffset = 5000.f;
	const FVector ForwardLocation = Owner->BulletTraceForwardLocation();
	//const FVector ForwardLocation = Owner->GetTargetCharacter()->GetActorLocation();
	const FVector StartLocation   = Owner->BulletTraceRelativeLocation();
	const FVector EndLocation = StartLocation + (ForwardLocation * ForwardOffset);

	FHitResult HitData(ForceInit);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.TraceTag = FName("");
	CollisionQueryParams.OwnerTag = FName("");
	CollisionQueryParams.bTraceAsyncScene = false;
	CollisionQueryParams.bTraceComplex = true;
	CollisionQueryParams.bFindInitialOverlaps = false;
	CollisionQueryParams.bReturnFaceIndex = false;
	CollisionQueryParams.bReturnPhysicalMaterial = false;
	CollisionQueryParams.bIgnoreBlocks = false;
	CollisionQueryParams.IgnoreMask = 0;
	CollisionQueryParams.AddIgnoredActor(this);

	const bool bSuccess = World->LineTraceSingleByChannel(
		HitData,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility,
		CollisionQueryParams);

	// draw
	//UKismetSystemLibrary::DrawDebugLine(World, StartLocation, EndLocation, FLinearColor(255, 0, 0, 0), 1.0f, 1.0f);

	if (!bSuccess)
	{
		return;
	}

	const FVector MuzzleLocation  = GetMuzzleTransform().GetLocation();
	const FRotator MuzzleRotation = FRotator(GetMuzzleTransform().GetRotation());
	UGameplayStatics::PlaySoundAtLocation(World, FireSoundAsset, MuzzleLocation, 1.f, 1.f, 0.f, nullptr, nullptr);
	Owner->FireActionMontage();
	--WeaponItemInfo.CurrentAmmo;

	UGameplayStatics::PlaySoundAtLocation(World, FireImpactSoundAsset, HitData.Location, 1.f, 1.f, 0.f, nullptr, nullptr);
	const FVector StartPoint = MuzzleLocation;
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(StartPoint, HitData.ImpactPoint);
	FTransform Transform = UKismetMathLibrary::MakeTransform(StartPoint, Rotation, FVector::OneVector);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	ABulletBase* const Bullet = World->SpawnActor<ABulletBase>(Super::BulletsBP, Transform, SpawnParams);

#if WITH_EDITOR
	Bullet->SetFolderPath("/BulletsRoot");
#endif

	Super::TakeHitDamage(HitData);
	Super::PlayBulletEffect(HitData);
}

void AAIWeaponBase::SetCharacterOwner(ACharacterBase* InCharacterOwner)
{
	Super::SetCharacterOwner(InCharacterOwner);

	if (AAICharacterBase* AICharacter = Cast<AAICharacterBase>(CharacterOwner))
	{
		Owner = AICharacter;
	}
	else
	{
		Owner = nullptr;
	}
}
