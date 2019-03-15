// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Wevet.h"
#include "Engine/World.h"
#include "SingletonLib.generated.h"

/**
 * 
 */
UCLASS()
class WEVET_API USingletonLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	template<class ActorType>
	static FORCEINLINE ActorType* GetSingletonActorInstance(ActorType* Instance)
	{
		if (Instance == nullptr) 
		{
			// Instance pointer not initialized

			// HACK: Using GameInstance as world context object...
			//       since it should always exist
			UWorld* const World = GetWorld();

			if (World) 
			{
				// Look for already existing instances
				TArray<AActor*> FoundActors;
				UGameplayStatics::GetAllActorsOfClass(World, ActorType::StaticClass(), FoundActors);

				if (FoundActors.Num() > 0) 
				{
					if (FoundActors.Num() > 1) 
					{
						UE_LOG(LogWevetClient, Error, TEXT("Multiple singleton instances found!"));
					}
					Instance = Cast<ActorType>(FoundActors[0]);
				}
				else 
				{
					// No instances exist; spawn new Actor
					// Look for BP class if it exists
					// (Assumes by naming standards that BP class name is of the format BP_<ClassName>)
					FName BPClassName = FName(*(FString::Printf(TEXT("BP_%s"), *(ActorType::StaticClass()->GetName()))));
					const FName SingletonBPDirectory = FName("Singletons");
					TSubclassOf<ActorType> BPClass = LoadGameBlueprintAsset<UClass>(SingletonBPDirectory, BPClassName);

					if (BPClass != nullptr) 
					{
						Instance = World->SpawnActor<ActorType>(BPClass);
					}
					else 
					{
						// No BP class; spawn C++ actor type
						Instance = World->SpawnActor<ActorType>();
					}
				}
			}
		}
		return Instance;
	}

	template <typename BPClass>
	static FORCEINLINE BPClass* LoadBlueprintAsset(const FName& Directory, const FName& Name)
	{
		if (Directory == NAME_None) 
		{
			return nullptr;
		}

		FString ClassName = Name.ToString().Append(TEXT("_C"));
		TArray<UObject*> Assets;
		if (EngineUtils::FindOrLoadAssetsByPath(*(Directory.ToString()), Assets, EngineUtils::EAssetToLoad::ATL_Class)) 
		{
			for (int i = 0, n = Assets.Num(); i < n; i++) 
			{
				UObject* Asset = Assets[i];

				if (Asset == nullptr) 
				{
					continue;
				}
				BPClass* Class = Cast<BPClass>(Asset);
				if (Class == nullptr || !Asset->GetName().Equals(ClassName)) 
				{
					continue;
				}
				return Class;
			}
		}
		return nullptr;
	}

	template <typename BPClass>
	static FORCEINLINE BPClass* LoadGameBlueprintAsset(const FName& Directory, const FName& Name)
	{
		FString FixedGameDirectory = FString::Printf(TEXT("/Game/Blueprints/%s"), *(Directory.ToString()));
		return LoadBlueprintAsset<BPClass>(
			FName(*(FixedGameDirectory)),
			Name
			);
	}

	template <typename BPClass>
	static FORCEINLINE BPClass* LoadPluginBlueprintAsset(const FName& PluginName, const FName& Directory, const FName& Id, const FName& Name)
	{
		FString FixedGameDirectory = FString::Printf(TEXT("/%s/Products/%s/%s/Blueprints"), *(PluginName.ToString()), *(Directory.ToString()), *(Id.ToString()));
		return LoadBlueprintAsset<BPClass>(
			FName(*(FixedGameDirectory)),
			Name
			);
	}

	template <typename BPClass>
	static FORCEINLINE BPClass* LoadPluginOrGameBlueprintAsset(const FName& PluginName, const FName& Directory, const FName& Id, const FName& Name)
	{
		if (PluginName.ToString().IsEmpty() || Id.ToString().IsEmpty())
		{
			return LoadGameBlueprintAsset<BPClass>(Directory, Name);
		}
		FString FixedGameDirectory = FString::Printf(TEXT("/%s/Products/%s/%s/Blueprints"), *(PluginName.ToString()), *(Directory.ToString()), *(Id.ToString()));
		return LoadBlueprintAsset<BPClass>(FName(*(FixedGameDirectory)), Name);
	}
};
