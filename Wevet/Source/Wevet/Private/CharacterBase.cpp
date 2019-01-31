// Copyright 2018 wevet works All Rights Reserved.

#include "CharacterBase.h"
#include "WeaponBase.h"
#include "ItemBase.h"
#include "CharacterModel.h"
#include "CharacterPickupComponent.h"
#include "CharacterAnimInstanceBase.h"

#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogWevetClient);

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	MovementSpeed(300.f),
	HeadSocketName(FName(TEXT("Head"))),
	PelvisSocketName(FName(TEXT("PelvisSocket"))),
	TakeDamageInterval(0.f),
	ComboTakeInterval(0.f)
{
	PrimaryActorTick.bCanEverTick = true;
	bCrouch  = false;
	bSprint  = false;
	bDied    = false;
	bHanging = false;
	bClimbingLedge = false;
	PawnNoiseEmitterComponent = ObjectInitializer.CreateDefaultSubobject<UPawnNoiseEmitterComponent>(this, TEXT("PawnNoiseEmitterComponent"));
	AudioComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->bAutoDestroy = false;
	AudioComponent->SetupAttachment(GetMesh());
	PickupComponent = ObjectInitializer.CreateDefaultSubobject<UCharacterPickupComponent>(this, TEXT("PickupComponent"));
}

void ACharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ACharacterBase::BeginDestroy()
{
	if (CharacterModel && CharacterModel->IsValidLowLevel())
	{
		CharacterModel->ConditionalBeginDestroy();
	}
	Super::BeginDestroy();
}

void ACharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Reset();
	}
	Super::EndPlay(EndPlayReason);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	DefaultMaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;

	if (ensure(PawnNoiseEmitterComponent && PawnNoiseEmitterComponent->IsValidLowLevel()))
	{
		//
	}
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TakeDamageInterval >= 0.f)
	{
		TakeDamageInterval -= DeltaTime;
	}
	if (ComboTakeInterval >= 0.f)
	{
		ComboTakeInterval -= DeltaTime;
	}
}

void ACharacterBase::OnReleaseItemExecuter_Implementation() 
{
}

void ACharacterBase::OnPickupItemExecuter_Implementation(AActor* Actor)
{
	if (Actor)
	{
		UE_LOG(LogWevetClient, Log, TEXT("Picking : %s"), *(Actor->GetName()));
	}
}

void ACharacterBase::ReportNoise_Implementation(USoundBase* Sound, float Volume)
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	if (Sound)
	{
		//MakeNoise(Volume, this, GetActorLocation());
		const float InVolume = FMath::Clamp<float>(Volume, 0.0f, 1.0f);
		UGameplayStatics::PlaySoundAtLocation(World, Sound, GetActorLocation(), InVolume, 1.0f, 0.0f, nullptr, nullptr);
		PawnNoiseEmitterComponent->MakeNoise(this, InVolume, GetActorLocation());
	}
}

void ACharacterBase::FootStep_Implementation(USoundBase* Sound, float Volume)
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const float Speed = GetVelocity().Size();
	const float InVolume = FMath::Clamp<float>((Speed / GetCharacterMovement()->MaxWalkSpeed), 0.f, 1.f);

	USoundBase* const InSound = Sound ? Sound : FootStepSoundAsset;
	if (InSound)
	{
		//MakeNoise(InVolume, this, GetActorLocation());
		UGameplayStatics::PlaySoundAtLocation(World, InSound, GetActorLocation(), InVolume, 1.0f, 0.0f, nullptr, nullptr);
		PawnNoiseEmitterComponent->MakeNoise(this, InVolume, GetActorLocation());
	}
}

void ACharacterBase::ReportNoiseOther_Implementation(AActor* Actor, USoundBase* Sound, const float Volume, const FVector Location)
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	if (Sound)
	{
		const float InVolume = FMath::Clamp<float>(Volume, 0.0f, 1.0f);
		UGameplayStatics::PlaySoundAtLocation(World, Sound, Location, InVolume, 1.0f, 0.0f, nullptr, nullptr);
		PawnNoiseEmitterComponent->MakeNoise(Actor, InVolume, Location);
	}
}

void ACharacterBase::CanGrab_Implementation(bool InCanGrab)
{
	bHanging = InCanGrab;
	if (bHanging)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	}
	IGrabExecuter::Execute_CanGrab(GetCharacterAnimInstance(), bHanging);
	UE_LOG(LogWevetClient, Log, TEXT("hanging : %s"), bHanging ? TEXT("true") : TEXT("false"));
}

bool ACharacterBase::IsDeath_Implementation()
{
	if (bDied || CharacterModel == nullptr)
	{
		return true;
	}
	return CharacterModel->GetCurrentHealth() <= 0;
}

void ACharacterBase::OnTakeDamage_Implementation(FName BoneName, float Damage, AActor* Actor)
{
	if (ICombatExecuter::Execute_IsDeath(this))
	{
		return;
	}

	if (BoneName == HeadSocketName) 
	{
		CharacterModel->SetCurrentHealthValue(0);
	} 
	else
	{
		int32 TakeDamage = (int32)(FMath::Abs(Damage));
		int32 CurrentHealth = CharacterModel->GetCurrentHealth();
		CharacterModel->SetCurrentHealthValue(CurrentHealth - TakeDamage);

		auto RefSkeleton = GetMesh()->SkeletalMesh->Skeleton->GetReferenceSkeleton();
		const int32 Index = RefSkeleton.FindBoneIndex(BoneName);
		if (Index >= 0)
		{

		}
		//UE_LOG(LogWevetClient, Log, TEXT("HitBoneName : %s"), *BoneName.ToString());
		TakeDamageActionMontage();
	}
}

void ACharacterBase::Die_Implementation()
{
	if (bDied)
	{
		return;
	}

	bDied = true;
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Reset();
	}
	USkeletalMeshComponent* const Mesh = GetMesh();
	Mesh->SetAllBodiesSimulatePhysics(true);
	Mesh->SetSimulatePhysics(true);
	Mesh->WakeAllRigidBodies();
	Mesh->bBlendPhysics = true;
	Super::GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UWorld* const World = GetWorld();
	check(World);

	const FRotator Rotation = Super::GetActorRotation();
	const FVector Forward   = Super::GetActorLocation() + (Controller->GetControlRotation().Vector() * DEFAULT_FORWARD);
	const FTransform Transform  = UKismetMathLibrary::MakeTransform(Forward, Rotation, FVector::OneVector);

	if (!ArrayExtension::NullOrEmpty(WeaponList))
	{
		for (AWeaponBase*& Weapon : WeaponList)
		{
			if (!Weapon)
			{
				continue;
			}
			ReleaseWeaponToWorld(Transform, Weapon);
		}
		WeaponList.Empty();
	}
}

void ACharacterBase::Equipment_Implementation()
{
	if (GetSelectedWeapon())
	{
		GetSelectedWeapon()->SetEquip(true);
	}
}

void ACharacterBase::UnEquipment_Implementation()
{
	if (GetSelectedWeapon())
	{
		GetSelectedWeapon()->SetEquip(false);
	}
}

FVector ACharacterBase::BulletTraceRelativeLocation() const
{
	return FVector::ZeroVector;
}

FVector ACharacterBase::BulletTraceForwardLocation() const
{
	return FVector::ZeroVector;
}

void ACharacterBase::Jump()
{
	if (bCrouch)
	{
		OnCrouch();
	}
	Super::Jump();
}

void ACharacterBase::StopJumping()
{
	Super::StopJumping();
}

void ACharacterBase::OnSprint()
{
	bSprint = !bSprint;

	// now crouching slow speed
	if (bCrouch)
	{
		bSprint = false;
	}
	MovementSpeed = bSprint ? DefaultMaxSpeed : DefaultMaxSpeed * 0.5f;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void ACharacterBase::OnCrouch()
{
	bCrouch = !bCrouch;
	Super::bIsCrouched = bCrouch ? 1 : 0;
	if (bCrouch)
	{
		if (Super::CanCrouch())
		{
			Super::Crouch();
		}
	}
	else
	{
		Super::UnCrouch();
	}
}

const bool ACharacterBase::HasEquipWeapon()
{
	if (GetSelectedWeapon())
	{
		return GetSelectedWeapon()->bEquip;
	}
	return false;
}

float ACharacterBase::GetHealthToWidget() const
{
	if (CharacterModel->IsValidLowLevel())
	{
		return CharacterModel->GetHealthToWidget();
	}
	return 0.f;
}

void ACharacterBase::ReleaseWeaponToWorld(const FTransform Transform, AWeaponBase* &Weapon)
{
	UWorld* const World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	const FWeaponItemInfo WeaponItemInfo = Weapon->WeaponItemInfo;
	TSubclassOf<class AWeaponBase> WeaponClass = WeaponItemInfo.WeaponClass;
	Weapon->Release(nullptr);
	Weapon = nullptr;

	AWeaponBase* const SpawningObject = World->SpawnActorDeferred<AWeaponBase>(
		WeaponClass,
		Transform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	SpawningObject->CopyWeaponItemInfo(WeaponItemInfo);
	SpawningObject->FinishSpawning(Transform);
}

AWeaponBase* ACharacterBase::FindByWeapon(const EWeaponItemType WeaponItemType)
{
	if (ArrayExtension::NullOrEmpty(WeaponList))
	{
		return nullptr;
	}

	for (AWeaponBase*& Weapon : WeaponList)
	{
		if (Weapon && Weapon->HasMatchTypes(WeaponItemType))
		{
			return Weapon;
		}
	}
	return nullptr;
}

AWeaponBase* ACharacterBase::GetSelectedWeapon() const
{
	if (CurrentWeapon.IsValid())
	{
		return CurrentWeapon.Get();
	}
	return nullptr;
}

UCharacterModel* ACharacterBase::GetCharacterModel() const
{
	if (CharacterModel->IsValidLowLevel())
	{
		return CharacterModel;
	}
	return nullptr;
}

UCharacterAnimInstanceBase* ACharacterBase::GetCharacterAnimInstance() const
{
	return Cast<UCharacterAnimInstanceBase>(GetMesh()->GetAnimInstance());
}

const TArray<AWeaponBase*>& ACharacterBase::GetWeaponList()
{
	return WeaponList;
}

const bool ACharacterBase::HasCrouch()
{
	return bCrouch;
}

const bool ACharacterBase::HasSprint()
{
	return bSprint;
}

const bool ACharacterBase::HasHanging()
{
	return bHanging;
}

AWeaponBase* ACharacterBase::GetUnEquipWeapon()
{
	if (ArrayExtension::NullOrEmpty(WeaponList))
	{
		return nullptr;
	}
	for (AWeaponBase*& Weapon : WeaponList)
	{
		if (Weapon && !Weapon->bEquip)
		{
			return Weapon;
		}
	}
	return nullptr;
}

void ACharacterBase::OutUnEquipWeaponList(TArray<AWeaponBase*>& OutWeaponList)
{
	if (ArrayExtension::NullOrEmpty(WeaponList))
	{
		return;
	}
	for (AWeaponBase* &Weapon : WeaponList)
	{
		if (Weapon && !Weapon->bEquip)
		{
			OutWeaponList.Emplace(Weapon);
		}
	}
}

const bool ACharacterBase::SameWeapon(AWeaponBase* const Weapon)
{
	const EWeaponItemType ItemType = Weapon->WeaponItemInfo.WeaponItemType;
	if (AWeaponBase* const InWeapon = FindByWeapon(ItemType))
	{
		return InWeapon->HasMatchTypes(ItemType);
	}
	return false;
}

void ACharacterBase::PickupObjects()
{
}

void ACharacterBase::ReleaseObjects()
{
}

void ACharacterBase::EquipmentActionMontage()
{
	if (ArrayExtension::NullOrEmpty(WeaponList))
	{
		return;
	}
	PlayAnimMontage(EquipMontage, MONTAGE_DELAY);
}

void ACharacterBase::UnEquipmentActionMontage()
{
	if (ArrayExtension::NullOrEmpty(WeaponList))
	{
		return;
	}
	PlayAnimMontage(UnEquipMontage, MONTAGE_DELAY);
}

void ACharacterBase::FireActionMontage()
{
	if (GetSelectedWeapon())
	{
		const EWeaponItemType WeaponType = GetSelectedWeapon()->WeaponItemInfo.WeaponItemType;
		switch (WeaponType)
		{
			case EWeaponItemType::Rifle:
			case EWeaponItemType::Sniper:
			{
				PlayAnimMontage(FireMontage);
			}
			break;
			case EWeaponItemType::Bomb:
			{
				//
			}
			break;
			case EWeaponItemType::Pistol:
			{
				//
			}
			break;
		}
	}
}

void ACharacterBase::ReloadActionMontage()
{
	if (GetSelectedWeapon())
	{
		const EWeaponItemType WeaponType = GetSelectedWeapon()->WeaponItemInfo.WeaponItemType;
		switch (WeaponType)
		{
			case EWeaponItemType::Rifle:
			case EWeaponItemType::Sniper:
			{
				PlayAnimMontage(ReloadMontage);
			}
			break;
			case EWeaponItemType::Bomb:
			{
				//
			}
			break;
			case EWeaponItemType::Pistol:
			{
				//
			}
			break;
		}
	}
}

void ACharacterBase::TakeDamageActionMontage()
{
	if (TakeDamageInterval <= 0)
	{
		if (GetSelectedWeapon())
		{
			const EWeaponItemType WeaponType = GetSelectedWeapon()->WeaponItemInfo.WeaponItemType;
			switch (WeaponType)
			{
			case EWeaponItemType::Rifle:
			case EWeaponItemType::Sniper:
				if (RifleHitDamageMontage)
				{
					TakeDamageInterval = PlayAnimMontage(RifleHitDamageMontage);
				}
				break;
			case EWeaponItemType::Bomb:
				//
				break;
			case EWeaponItemType::Pistol:
				//
				break;
			};
		}
		else
		{
			if (DefaultHitDamageMontage)
			{
				TakeDamageInterval = PlayAnimMontage(DefaultHitDamageMontage);
			}
		}
	}
}