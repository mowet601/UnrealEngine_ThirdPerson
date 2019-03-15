// Copyright © 2018 wevet works All Rights Reserved.

#include "EnvironmentPawn.h"
#include "Wevet.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AEnvironmentPawn::AEnvironmentPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	StaticMeshComponent(nullptr),
	SphereComponent(nullptr),
	bPlaying(false)
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(90.0f);
	SphereComponent->SetupAttachment(RootComponent);

	StaticMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SphereComponent);
	StaticMeshComponent->bRenderCustomDepth = true;
	StaticMeshComponent->CustomDepthStencilValue = 1;
	PawnNoiseEmitterComponent = ObjectInitializer.CreateDefaultSubobject<UPawnNoiseEmitterComponent>(this, TEXT("PawnNoiseEmitterComponent"));
}

void AEnvironmentPawn::BeginPlay()
{
	Super::BeginPlay();
	ParticleSystemComponents = Wevet::ComponentExtension::GetComponentsArray<UParticleSystemComponent>(this);
	UE_LOG(LogWevetClient, Log, TEXT("ParticleNum : %d"), ParticleSystemComponents.Num());
}

void AEnvironmentPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnvironmentPawn::BeginOverlapRecieve(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if (!bPlaying)
	{
		bPlaying = true;
		UWorld* const World = GetWorld();
		if (World)
		{
			const FTransform Transform = GetActorTransform();
			StartEffect();
			UGameplayStatics::PlaySoundAtLocation(
				World, 
				EffectSoundAsset, 
				Transform.GetLocation(), 
				DEFAULT_VOLUME, 
				1.0f, 
				0.0f, 
				nullptr, 
				nullptr);
			PawnNoiseEmitterComponent->MakeNoise(this, DEFAULT_VOLUME, Transform.GetLocation());
		}
	}
}

void AEnvironmentPawn::EndOverlapRecieve(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	if (bPlaying)
	{
		bPlaying = false;
		StopEffect();
	}
}

void AEnvironmentPawn::StartEffect()
{
	if (ParticleSystemComponents.Num() <= 0)
	{
		return;
	}
	for (UParticleSystemComponent* &Component : ParticleSystemComponents)
	{
		if (Component)
		{
			Component->ActivateSystem();
		}
	}
}

void AEnvironmentPawn::StopEffect()
{
	if (ParticleSystemComponents.Num() <= 0)
	{
		return;
	}
	for (UParticleSystemComponent* &Component : ParticleSystemComponents)
	{
		if (Component)
		{
			Component->DeactivateSystem();
		}
	}
}

//void AEnvironmentPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//}

