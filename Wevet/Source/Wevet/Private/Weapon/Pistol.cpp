// Copyright © 2018 wevet works All Rights Reserved.


#include "Weapon/Pistol.h"
#include "Weapon/BulletBase.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"
#include "Character/CharacterBase.h"

APistol::APistol(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BulletDuration = 0.15f;
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent->SetBoxExtent(FVector(10.f, 24.f, 20.f));

	MeleeCollisionComponent->SetRelativeLocation(FVector(0.0f, 9.f, 13.f));
	MeleeCollisionComponent->SetBoxExtent(FVector(2.f, 13.f, 6.f));

	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Weapons/Pistols_A"));
		USkeletalMesh* Asset = FindAsset.Object;
		SkeletalMeshComponent->SetSkeletalMesh(Asset);
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Sound/Pistol/Cues/PistolA_Fire_Cue"));
		FireSound = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<USoundBase> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/Sound/Pistol/Cues/Pistol_Whip_Cue"));
		ImpactSound = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/FX/P_Pistol_MuzzleFlash_01"));
		FlashEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> FindAsset(TEXT("/Game/Assets/MilitaryWeapon/FX/P_Impact_Metal_Small_01"));
		ImpactEmitterTemplate = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UClass> FindAsset(TEXT("/Game/Game/Blueprints/Bullet/BP_Pistol_Bullet.BP_Pistol_Bullet_C"));
		BulletsTemplate = FindAsset.Object;
	}

	// Colt Government
	NeededAmmo = 8;
	WeaponItemInfo.MaxAmmo = 200;
	WeaponItemInfo.EquipSocketName = FName(TEXT("PistolEquip_Socket"));
	WeaponItemInfo.UnEquipSocketName = FName(TEXT("Pistol_Socket"));
	WeaponItemInfo.WeaponItemType = EWeaponItemType::Pistol;
	WeaponItemInfo.DisplayName = FString(TEXT("Colt Government"));
	WeaponItemInfo.MeleeDistance = 1000.f;
}

