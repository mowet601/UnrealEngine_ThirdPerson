// Copyright © 2018 wevet works All Rights Reserved.

#include "MainLevelScriptActor.h"
#include "MockCharacter.h"
#include "AICharacterBase.h"
#include "Engine.h"


AMainLevelScriptActor::AMainLevelScriptActor(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CharacterArray.Reset(0);
	AICharacterArray.Reset(0);
}

void AMainLevelScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AMainLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	CreateCharacter();
}

void AMainLevelScriptActor::CreateCharacter()
{
	UWorld* const World = GetWorld();

	if (World == nullptr)
	{
		return;
	}
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ACharacterBase::StaticClass(), FoundActors);

	for (AActor*& Actor : FoundActors)
	{
		if (AMockCharacter* Character = Cast<AMockCharacter>(Actor))
		{
			CharacterArray.Emplace(Character);
		}
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

