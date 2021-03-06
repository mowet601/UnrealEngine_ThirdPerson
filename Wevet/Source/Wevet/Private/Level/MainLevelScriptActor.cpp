// Copyright © 2018 wevet works All Rights Reserved.

#include "Level/MainLevelScriptActor.h"
#include "Player/MockCharacter.h"
#include "AI/AICharacterBase.h"
#include "Engine.h"
#include "WevetExtension.h"


AMainLevelScriptActor::AMainLevelScriptActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	PlayerCharacter(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMainLevelScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	PlayerCharacter = nullptr;
	CharacterArray.Reset(0);
	AICharacterArray.Reset(0);
	Super::EndPlay(EndPlayReason);
}

void AMainLevelScriptActor::BeginPlay()
{
	Super::SetActorTickEnabled(false);
	Super::BeginPlay();
	SetPlayer();
}

void AMainLevelScriptActor::SetPlayer()
{
	PlayerCharacter = Cast<AMockCharacter>(Wevet::ControllerExtension::GetPlayerPawn(this));
}

void AMainLevelScriptActor::CreateCharacter()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterBase::StaticClass(), FoundActors);

	for (AActor*& Actor : FoundActors)
	{
		if (AAICharacterBase* AICharacter = Cast<AAICharacterBase>(Actor))
		{
			AICharacterArray.Emplace(AICharacter);
		}
	}
}

const TArray<ACharacterBase*>& AMainLevelScriptActor::GetCharacterArray()
{
	return CharacterArray;
}

const TArray<AAICharacterBase*>& AMainLevelScriptActor::GetAICharacterArray()
{
	return AICharacterArray;
}

