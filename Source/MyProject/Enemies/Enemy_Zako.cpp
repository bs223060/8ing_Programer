#include "Enemy_Zako.h"
#include "Kismet/GameplayStatics.h"
#include "MyProject/Core/MyGameInstance.h"
#include "AIController.h"



AEnemy_Zako::AEnemy_Zako()
{
	PrimaryActorTick.bCanEverTick = false;
	
	//  バトルトリガーのセットアップ
	BattleTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("BattleTrigger"));
	BattleTrigger->SetupAttachment(RootComponent);
	BattleTrigger->SetSphereRadius(250.0f);

	//  衝突設定
	BattleTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BattleTrigger->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BattleTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BattleTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	BattleTrigger->SetGenerateOverlapEvents(true);

	//  AIコントローラーの設定
	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	MaxHP = 100.0f;
}



// ボス死亡時に自動で消滅するかをチェックする関数
void AEnemy_Zako::CheckAndDestroySelf()
{
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());

	if (GI != nullptr)
	{
		// ボス死亡時のみC++側で消滅を管理
		if (GI->bIsBossDead)
		{
			// バトルトリガーが存在する場合は先に破壊
			if (BattleTrigger != nullptr)
			{

				BattleTrigger->DestroyComponent();
			}

			Destroy();
		}
	}
}



void AEnemy_Zako::BeginPlay()
{

	Super::BeginPlay();
	CurrentHP = MaxHP;

	// バトルトリガーのオーバーラップイベントをバインド
	if (BattleTrigger != nullptr)
	{
		BattleTrigger->OnComponentBeginOverlap.AddDynamic(this, &AEnemy_Zako::OnOverlapBegin);
	}

	// ボス死亡チェック用
	CheckAndDestroySelf();
}



float AEnemy_Zako::GetHealthPercentage() const
{
	return (MaxHP > 0.0f) ? (CurrentHP / MaxHP) : 0.0f;
}



void AEnemy_Zako::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor != nullptr)
	{

		if (OtherActor != this) 
		{
			// 相手がプレイヤーかチェック
			if (OtherActor->ActorHasTag("Player")) 
			{
				// GameInstanceを取得して、敵IDを保存
				UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
				
				if (GI != nullptr)
				{
					GI->CurrentEnemyID = this->EnemyID;
					GI->IsBossBattle = false;
					GI->PlayerPreBattleTransform = OtherActor->GetActorTransform();
					GI->EnemyClassToSpawn = this->GetClass();

					// 倒した敵の名前を保存
					GI->DefeatedEnemyNames.AddUnique(this->EnemyUniqueName);

					// 戦闘から戻るときのフラグをオン
					GI->bIsReturningFromBattle = true;

					UGameplayStatics::OpenLevel(this, FName("L_Battle"));
				}
			}
			
		}
		
	}
}



float AEnemy_Zako::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{

	if (CurrentHP <= 0.0f)
	{
		return 0.0f;
	}

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHP <= 0.0f)
	{
		// HPが0以下になった場合の処理
		UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
		if (GI != nullptr)
		{
			// 戻りフラグをオン
			GI->bIsReturningFromBattle = true;

			// 戦闘勝利フラグをオン
			GI->bIsBattleWon = true;

			// 倒した敵の名前を保存
			GI->DefeatedEnemyNames.AddUnique(this->EnemyUniqueName);
		}

	}

	return ActualDamage;
}