// Copyright © 2018 wevet works All Rights Reserved.


#include "Weapon/AbstractWeapon.h"
#include "Character/CharacterBase.h"
#include "Interface/AIPawnOwner.h"

AAbstractWeapon::AAbstractWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	MuzzleSocketName(FName(TEXT("MuzzleFlash")))
{
	PrimaryActorTick.bCanEverTick = true;
	bFirePressed = false;
	bEquip  = false;
	bReload = false;

	// 
	SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));
	RootComponent = SceneComponent;
}

void AAbstractWeapon::BeginPlay()
{
	Super::BeginPlay();
	Super::SetActorTickEnabled(false);
}

void AAbstractWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ResetCharacterOwner();
	PrepareDestroy();
	Super::EndPlay(EndPlayReason);
}

void AAbstractWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

ACharacterBase* AAbstractWeapon::GetPointer() const
{
	if (CharacterPtr.IsValid())
	{
		return CharacterPtr.Get();
	}
	return nullptr;
}

APawn* AAbstractWeapon::GetPawnOwner() const
{
	if (CharacterPtr.IsValid())
	{
		return CharacterPtr.Get();
	}
	return nullptr;
}

void AAbstractWeapon::Initialize(ACharacterBase* const NewCharacterOwner)
{
	CharacterPtr = MakeWeakObjectPtr<ACharacterBase>(NewCharacterOwner);
	if (GetPawnOwner())
	{
		IgnoreActors.Add(GetPawnOwner());
		IgnoreActors.Add(this);
		Super::SetOwner(GetPawnOwner());
	}
}

void AAbstractWeapon::ResetCharacterOwner()
{
	CharacterPtr.Reset();
	IgnoreActors.Reset(0);
	Super::SetOwner(nullptr);
}

void AAbstractWeapon::DoFirePressed_Implementation() 
{
	bFirePressed = true;
}

void AAbstractWeapon::DoFireRelease_Implementation()
{
	bFirePressed = false;
}

// Reload
void AAbstractWeapon::DoReload_Implementation()
{

}

// 補給
void AAbstractWeapon::DoReplenishment_Implementation(const FWeaponItemInfo& RefWeaponItemInfo)
{
	WeaponItemInfo.MaxAmmo += RefWeaponItemInfo.MaxAmmo;
	NeededAmmo = (WeaponItemInfo.ClipType - WeaponItemInfo.CurrentAmmo);
	if (WeaponItemInfo.MaxAmmo <= NeededAmmo)
	{
		WeaponItemInfo.MaxAmmo = 0;
		WeaponItemInfo.CurrentAmmo = (WeaponItemInfo.CurrentAmmo + WeaponItemInfo.MaxAmmo);
	}
	else
	{
		WeaponItemInfo.MaxAmmo = (WeaponItemInfo.MaxAmmo - NeededAmmo);
		WeaponItemInfo.CurrentAmmo = WeaponItemInfo.ClipType;
	}
}

bool AAbstractWeapon::CanMeleeStrike_Implementation() const
{
	return false;
}

void AAbstractWeapon::Take_Implementation(ACharacterBase* NewCharacter)
{
	ResetCharacterOwner();
	Initialize(NewCharacter);
}

void AAbstractWeapon::Release_Implementation(ACharacterBase* NewCharacter)
{
	ResetCharacterOwner();
	SetEquip(false);

	if (IsValidLowLevel())
	{
		PrepareDestroy();
		Super::Destroy();
		Super::ConditionalBeginDestroy();
	}	
}

void AAbstractWeapon::CheckIfValid(AActor* OtherActor, bool& bCanDamageResult)
{
	if (OtherActor == nullptr)
	{
		return;
	}

	// Owner nullptr || Same Owner Hit Actor
	if (GetPawnOwner() == nullptr || (GetPawnOwner() && GetPawnOwner() == OtherActor))
	{
		return;
	}

	IDamageInstigator* DamageInstigator = Cast<IDamageInstigator>(OtherActor);
	if (DamageInstigator == nullptr)
	{
		return;
	}

	// Already Death
	if (IDamageInstigator::Execute_IsDeath(DamageInstigator->_getUObject()))
	{
		return;
	}

	// if AIOwner Overlap hit AIOwner
	if (IAIPawnOwner* AI = Cast<IAIPawnOwner>(GetPawnOwner()))
	{
		if (IAIPawnOwner* InstigatorAI = Cast<IAIPawnOwner>(OtherActor))
		{
			return;
		}
	}
	bCanDamageResult = true;
}

void AAbstractWeapon::HitReceive(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
}

void AAbstractWeapon::BeginOverlapRecieve(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
}

void AAbstractWeapon::EndOverlapRecieve(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

void AAbstractWeapon::TakeDamageOuter(AActor* OtherActor, const FName BoneName, const float AdditionalDamage)
{
	if (OtherActor == nullptr || GetPawnOwner() == nullptr)
	{
		return;
	}

	// Same Actor or Hit Actor Owner
	if ((OtherActor && OtherActor == this) || (OtherActor && OtherActor == GetPawnOwner()))
	{
		return;
	}

	if (!OtherActor->IsValidLowLevel())
	{
		return;
	}

	IDamageInstigator* DamageInstigator = Cast<IDamageInstigator>(OtherActor);
	if (DamageInstigator == nullptr)
	{
		return;
	}

	float TotalDamage = ZERO_VALUE;
	UCharacterModel* const DamageModel = IDamageInstigator::Execute_GetPropertyModel(DamageInstigator->_getUObject());

	// 1 Create Damage Parameter
	CreateDamage(DamageModel, AdditionalDamage, TotalDamage);
	UE_LOG(LogWevetClient, Log, TEXT("BoneName : %s"), *BoneName.ToString());

	// 2 Check Kill
	bool bHasDie = false;
	IDamageInstigator::Execute_OnTakeDamage(DamageInstigator->_getUObject(), BoneName, TotalDamage, GetPawnOwner(), bHasDie);
	IDamageInstigator::Execute_InfrictionDamage(GetPawnOwner(), OtherActor, bHasDie);
}

void AAbstractWeapon::CreateDamage(UCharacterModel* DamageModel, const int InWeaponDamage, float& OutDamage)
{
	if (GetPawnOwner())
	{
		IDamageInstigator::Execute_ApplyDamage(GetPawnOwner(), DamageModel, InWeaponDamage, OutDamage);
	}
}

void AAbstractWeapon::OnFirePressInternal()
{
	//
}

void AAbstractWeapon::OnFireReleaseInternal()
{
	//
}

void AAbstractWeapon::OnReloadInternal()
{
	bFirePressed = false;
	NeededAmmo = (WeaponItemInfo.ClipType - WeaponItemInfo.CurrentAmmo);

	if (WeaponItemInfo.MaxAmmo <= NeededAmmo)
	{
		WeaponItemInfo.MaxAmmo = 0;
		WeaponItemInfo.CurrentAmmo = (WeaponItemInfo.CurrentAmmo + WeaponItemInfo.MaxAmmo);
	}
	else
	{
		WeaponItemInfo.MaxAmmo = (WeaponItemInfo.MaxAmmo - NeededAmmo);
		WeaponItemInfo.CurrentAmmo = WeaponItemInfo.ClipType;
	}
}

void AAbstractWeapon::SetEquip(const bool InEquip)
{
	bEquip = InEquip;
}

void AAbstractWeapon::SetReload(const bool InReload)
{
	bReload = InReload;
}

void AAbstractWeapon::PrepareDestroy()
{
	UWorld* const World = GetWorld();
	if (World && World->GetTimerManager().IsTimerActive(ReloadTimerHandle))
	{
		World->GetTimerManager().ClearTimer(ReloadTimerHandle);
	}
}

FTransform AAbstractWeapon::GetMuzzleTransform() const
{
	return FTransform::Identity;
}

void AAbstractWeapon::PlayEffect(const FHitResult& HitResult, USceneComponent* const InComponent)
{
	FTransform EmitterTransform;
	EmitterTransform.SetIdentity();
	EmitterTransform.SetLocation(HitResult.Location);

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		ImpactEmitterTemplate,
		EmitterTransform,
		true);

	UGameplayStatics::SpawnEmitterAttached(
		FlashEmitterTemplate,
		InComponent,
		MuzzleSocketName,
		GetMuzzleTransform().GetLocation(),
		FRotator(GetMuzzleTransform().GetRotation()),
		EAttachLocation::KeepWorldPosition,
		true);
}

bool AAbstractWeapon::WasEquip() const
{
	return bEquip;
}

bool AAbstractWeapon::WasReload() const
{
	return bReload;
}

bool AAbstractWeapon::WasSameWeaponType(EWeaponItemType InWeaponItemType) const
{
	if (WeaponItemInfo.WeaponItemType == EWeaponItemType::None)
	{
		return false;
	}
	return WeaponItemInfo.WeaponItemType == InWeaponItemType;
}

void AAbstractWeapon::CopyWeaponItemInfo(const FWeaponItemInfo RefWeaponItemInfo)
{
	WeaponItemInfo.UnEquipSocketName = RefWeaponItemInfo.UnEquipSocketName;
	WeaponItemInfo.EquipSocketName = RefWeaponItemInfo.EquipSocketName;
	WeaponItemInfo.WeaponItemType = RefWeaponItemInfo.WeaponItemType;
	WeaponItemInfo.CurrentAmmo = RefWeaponItemInfo.CurrentAmmo;
	WeaponItemInfo.ClipType = RefWeaponItemInfo.ClipType;
	WeaponItemInfo.MaxAmmo = RefWeaponItemInfo.MaxAmmo;
	WeaponItemInfo.Damage = RefWeaponItemInfo.Damage;
	WeaponItemInfo.Texture = RefWeaponItemInfo.Texture;
}
