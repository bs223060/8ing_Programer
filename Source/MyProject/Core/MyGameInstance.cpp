#include "MyGameInstance.h"

// ゲームオーバー時にプレイヤーのデータを初期値にリセットする関数
void UMyGameInstance::ResetPlayerStats()
{
	StoredLevel = 1;
	StoredMaxHP = 150.0f;
	StoredHP = 150.0f;

	StoredCurrentExp = 0;
	StoredAttackDamage = 10.0f;
	bIsBossDead = false;
	IsBossBattle = false;
	DefeatedEnemyNames.Empty();
}