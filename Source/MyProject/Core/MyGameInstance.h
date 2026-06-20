#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

UCLASS()
class MYPROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	FVector LastPlayerLocation;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	FTransform PlayerPreBattleTransform;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	bool bIsReturningFromBattle = false;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	bool bIsBossDead = false;

	UPROPERTY(BlueprintReadWrite, Category = "GameData")
	bool IsBossBattle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float StoredHP = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 CurrentEnemyID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	TSubclassOf<AActor> EnemyClassToSpawn;

	UPROPERTY(BlueprintReadWrite, Category = "Battle")
	TArray<FName> DefeatedEnemyNames;

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void ClearDefeatedEnemyNames() { DefeatedEnemyNames.Empty(); }

	UFUNCTION(BlueprintCallable, Category = "SaveData")
	void ResetPlayerStats();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float StoredMaxHP = 150.0f;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerStats")
	int32 StoredLevel = 1; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float StoredAttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	int32 StoredCurrentExp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	bool bIsBattleWon = false;
};