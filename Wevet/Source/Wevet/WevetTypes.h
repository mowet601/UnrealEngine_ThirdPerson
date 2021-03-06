#pragma once
#include "Engine/EngineTypes.h"
#include "WevetTypes.generated.h"

/** List of versions, native code will handle fixups for any old versions */
namespace ESaveGameVersion
{
	enum type
	{
		// Initial version
		Initial,
		// Added Inventory
		AddedInventory,
		// Added ItemData to store count/level
		AddedItemData,
		// -----<new versions must be added before this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};
}

namespace Wevet
{
	enum BattlePhase
	{
		Normal,
		Warning,
		Alert,
	};

}

UENUM()
enum class ECustomDepthType : uint8
{
	None = 0,
	Environment = 1,
	Weapon = 10,
	Item = 20,
	Pawn = 30,
	//Max = 256,
};

UENUM(BlueprintType)
enum class EAIActionState : uint8
{
	None  UMETA(DisplayName = "None"),
	Observation  UMETA(DisplayName = "Observation"),
	Attack  UMETA(DisplayName = "Attack"),
	ConditionCheck UMETA(DisplayName = "ConditionCheck"),
	Cover UMETA(DisplayName = "Cover"),
};

UENUM()
enum class EBattlePhaseType : uint8
{
	Normal  UMETA(DisplayName = "Normal"),
	Warning UMETA(DisplayName = "Warning"),
	Alert   UMETA(DisplayName = "Alert"),
};

UENUM()
enum class EBotBehaviorType : uint8
{
	Passive,
	Patrolling,
};

// UI ManagerLayer
UENUM()
enum class EUMGLayerType : int32
{
	None = -1,
	WorldScreen = 0,
	Main = 1,
	Overlay = 100,
};

UENUM(BlueprintType)
enum class EWeaponItemType : uint8
{
	None   UMETA(DisplayName = "None"),
	Pistol UMETA(DisplayName = "Pistol"),
	Rifle  UMETA(DisplayName = "Rifle"),
	Sniper UMETA(DisplayName = "Sniper"),
	Knife  UMETA(DisplayName = "Knife"),
	Bomb   UMETA(DisplayName = "Bomb"),
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None   UMETA(DisplayName = "None"),
	Weapon UMETA(DisplayName = "Weapon"), // AAbstractWeapon
	Health UMETA(DisplayName = "Health"), 
	Ammos  UMETA(DisplayName = "Ammos"),  // AAbstractItem
};

UENUM(BlueprintType)
enum class EGiveDamageType : uint8
{
	None   UMETA(DisplayName = "None"),
	Shoot  UMETA(DisplayName = "Shoot"),
	Melee  UMETA(DisplayName = "Melee"),
};

//UENUM(BlueprintType)
//enum class EWeaponState : uint8
//{
//	None    UMETA(DisplayName = "None"),
//	Empty   UMETA(DisplayName = "Empty"),
//	Reload  UMETA(DisplayName = "Reload"),
//	Shoot   UMETA(DisplayName = "Shoot"),
//	Melee   UMETA(DisplayName = "Melee"),
//	NoOwner UMETA(DisplayName = "NoOwner"),
//};
