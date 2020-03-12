// Copyright © 2018 wevet works All Rights Reserved.


#include "Weapon/Pistol.h"
#include "Weapon/BulletBase.h"
#include "Character/CharacterBase.h"
#include "Component/CharacterPickupComponent.h"
#include "Character/CharacterModel.h"
#include "Player/MockCharacter.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "WevetExtension.h"

APistol::APistol(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BulletDuration = 0.1f;
	ReloadDuration = 0.2f;
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(90.0f);
	SphereComponent->SetupAttachment(RootComponent);

	SkeletalMeshComponent = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(SphereComponent);
	SkeletalMeshComponent->bRenderCustomDepth = false;
	SkeletalMeshComponent->CustomDepthStencilValue = 1;

	WidgetComponent = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("WidgetComponent"));
	WidgetComponent->SetDrawSize(FVector2D(180.f, 70.f));
	WidgetComponent->SetWorldLocation(FVector(0.f, 0.f, 60.f));
	WidgetComponent->SetupAttachment(SphereComponent);

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

	NeededAmmo = 50;
	WeaponItemInfo.UnEquipSocketName = NAME_None;
}

void APistol::BeginPlay()
{
	Super::BeginPlay();

	if (Wevet::ComponentExtension::HasValid(WidgetComponent))
	{
		WidgetComponent->SetVisibility(false);
	}

	if (Wevet::ComponentExtension::HasValid(SphereComponent))
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APistol::BeginOverlapRecieve);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this, &APistol::EndOverlapRecieve);
	}
}

void APistol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APistol::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	PrepareDestroy();
	Super::EndPlay(EndPlayReason);
}

bool APistol::CanMeleeStrike_Implementation() const
{
	// was nullptr
	if (!Super::CharacterPtr.IsValid())
	{
		return false;
	}

	// was death
	if (IDamageInstigator::Execute_IsDeath(CharacterPtr.Get()))
	{
		return false;
	}
	bool bWasEmpty = (WeaponItemInfo.MaxAmmo <= 0);
	return (bWasEmpty == false);
}

void APistol::BeginOverlapRecieve(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (GetOwner())
	{
		UE_LOG(LogWevetClient, Warning, TEXT("Already Owner : %s"), *GetOwner()->GetName());
		return;
	}

	if (IInteractionPawn* Interface = Cast<IInteractionPawn>(OtherActor))
	{
		if (!IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
		{
			return;
		}
	}

	if (AMockCharacter* Character = Cast<AMockCharacter>(OtherActor))
	{
		WidgetComponent->SetVisibility(true);
		SkeletalMeshComponent->SetRenderCustomDepth(true);
		Character->GetPickupComponent()->SetPickupActor(this);
	}
}

void APistol::EndOverlapRecieve(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (GetOwner())
	{
		UE_LOG(LogWevetClient, Warning, TEXT("Already Owner : %s"), *GetOwner()->GetName());
		return;
	}

	if (IInteractionPawn * Interface = Cast<IInteractionPawn>(OtherActor))
	{
		if (!IInteractionPawn::Execute_CanPickup(Interface->_getUObject()))
		{
			return;
		}
	}

	if (AMockCharacter* Character = Cast<AMockCharacter>(OtherActor))
	{
		WidgetComponent->SetVisibility(false);
		SkeletalMeshComponent->SetRenderCustomDepth(false);
		Character->GetPickupComponent()->SetPickupActor(nullptr);
	}
}

void APistol::OnFirePressInternal()
{
	UWorld* const World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	if (!CanMeleeStrike_Implementation())
	{
		return;
	}

	if (Super::bReload)
	{
		return;
	}
	if (WeaponItemInfo.CurrentAmmo <= 0)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Out Of Ammos"));
		DoReload_Implementation();
		return;
	}

	const FVector StartLocation = CharacterPtr.Get()->BulletTraceRelativeLocation();
	const FVector ForwardLocation = CharacterPtr.Get()->BulletTraceForwardLocation();
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
	CollisionQueryParams.AddIgnoredActors(IgnoreActors);

	const bool bSuccess = World->LineTraceSingleByChannel(
		HitData,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Camera,
		CollisionQueryParams);

	const FVector MuzzleLocation = GetMuzzleTransform().GetLocation();
	const FRotator MuzzleRotation = FRotator(GetMuzzleTransform().GetRotation());
	ISoundInstigator::Execute_ReportNoiseOther(CharacterPtr.Get(), this, FireSound, DEFAULT_VOLUME, MuzzleLocation);
	CharacterPtr.Get()->FireActionMontage();
	--WeaponItemInfo.CurrentAmmo;

	ISoundInstigator::Execute_ReportNoiseOther(CharacterPtr.Get(), this, ImpactSound, DEFAULT_VOLUME, HitData.Location);
	const FVector StartPoint = MuzzleLocation;
	const FVector EndPoint = UKismetMathLibrary::SelectVector(HitData.ImpactPoint, HitData.TraceEnd, bSuccess);
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(StartPoint, EndPoint);
	FTransform Transform = UKismetMathLibrary::MakeTransform(StartPoint, Rotation, FVector::OneVector);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	ABulletBase* const Bullet = World->SpawnActor<ABulletBase>(Super::BulletsTemplate, Transform, SpawnParams);

#if WITH_EDITOR
	Bullet->SetFolderPath("/BulletsRoot");
#endif

	bool bCanDamageResult = false;
	Super::CheckIfValid(HitData.GetActor(), bCanDamageResult);
	if (bCanDamageResult)
	{
		Super::TakeDamageOuter(HitData.GetActor(), HitData.BoneName, WeaponItemInfo.Damage);
	}
	Super::PlayEffect(HitData, SkeletalMeshComponent);
}

void APistol::DoReload_Implementation()
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	if (WeaponItemInfo.MaxAmmo <= 0)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Empty Ammos Current:%d, ClipType:%d"), WeaponItemInfo.CurrentAmmo, WeaponItemInfo.ClipType);
		return;
	}

	if (WeaponItemInfo.CurrentAmmo >= WeaponItemInfo.ClipType)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Full Ammos Current:%d, ClipType:%d"), WeaponItemInfo.CurrentAmmo, WeaponItemInfo.ClipType);
		return;
	}

	SetReload(true);
	CharacterPtr.Get()->ReloadActionMontage(ReloadDuration);

	OnReloadInternal();
	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([&]
	{
		SetReload(false);
	});
	World->GetTimerManager().SetTimer(ReloadTimerHandle, TimerCallback, ReloadDuration, false);
}

void APistol::Take_Implementation(ACharacterBase* NewCharacter)
{
	Super::Take_Implementation(NewCharacter);
	SetEquip(false);

	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMeshComponent->SetSimulatePhysics(false);
	WidgetComponent->SetVisibility(false);
	SphereComponent->Deactivate();

	if (Wevet::ComponentExtension::HasValid(SphereComponent))
	{
		SphereComponent->OnComponentBeginOverlap.RemoveDynamic(this, &APistol::BeginOverlapRecieve);
		SphereComponent->OnComponentEndOverlap.RemoveDynamic(this, &APistol::EndOverlapRecieve);
	}

	UWorld* const World = GetWorld();

	if (PickupSound && World)
	{
		UGameplayStatics::PlaySoundAtLocation(World, PickupSound, GetActorLocation());
	}
	SkeletalMeshComponent->SetRenderCustomDepth(false);
}

FTransform APistol::GetMuzzleTransform() const
{
	return SkeletalMeshComponent->GetSocketTransform(MuzzleSocketName);
}

void APistol::SetEquip(const bool InEquip)
{
	Super::SetEquip(InEquip);
	if (WasEquip())
	{
		SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SkeletalMeshComponent->SetSimulatePhysics(false);
		WidgetComponent->SetVisibility(false);
		SphereComponent->Deactivate();

		if (SphereComponent->OnComponentBeginOverlap.IsBound())
		{
			SphereComponent->OnComponentBeginOverlap.RemoveDynamic(this, &APistol::BeginOverlapRecieve);
		}
		if (SphereComponent->OnComponentEndOverlap.IsBound())
		{
			SphereComponent->OnComponentEndOverlap.RemoveDynamic(this, &APistol::EndOverlapRecieve);
		}
	}
}

