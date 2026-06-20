#include "MyBossEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "../Core/MyGameInstance.h"
#include "Enemy_Zako.h"



AMyBossEnemy::AMyBossEnemy()
{
	//  Tickを有効化
    PrimaryActorTick.bCanEverTick = true;

    // バトルトリガーのセットアップ
    BattleTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BattleTrigger"));
    BattleTrigger->SetupAttachment(RootComponent);
    BattleTrigger->SetBoxExtent(FVector(200.0f, 2000.0f, 500.0f));

	// 衝突設定
    BattleTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BattleTrigger->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    BattleTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    BattleTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    BattleTrigger->SetGenerateOverlapEvents(true);

	// AIコントローラーの設定
    bIsCharging = false;

	// 初期ステータスの設定
    MaxHP = 250.0f;
}


void AMyBossEnemy::BeginPlay()
{

    Super::BeginPlay();
    CurrentHP = MaxHP;

    if (BattleTrigger)
    {
        BattleTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMyBossEnemy::OnOverlapBegin);
    }
}



void AMyBossEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    if (OtherActor == nullptr || OtherActor == this)
    {
        return;
    }

    if (OtherActor->ActorHasTag("Player"))
    {
		
        UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

        if (GI != nullptr)
        {
            GI->IsBossBattle = true;
            GI->PlayerPreBattleTransform = OtherActor->GetActorTransform();
            GI->EnemyClassToSpawn = this->GetClass();

			// 倒した敵の名前の保存
            GI->DefeatedEnemyNames.AddUnique(this->BossUniqueName);

			// 戦闘から戻るときのフラグをオン
            UGameplayStatics::OpenLevel(this, FName("L_Battle"));
        }
    }
    
}



float AMyBossEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{

    double CurrentTime = GetWorld()->GetTimeSeconds();

    if (CurrentTime - LastDamageTime < 0.1)
    {
        return 0.0f;
    }

    LastDamageTime = CurrentTime;

    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (this->CurrentHP <= 0.0f)
    {
        ABattleCharacter* Player = Cast<ABattleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

        TArray<AActor*> FoundEnemies;

        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABattleCharacter::StaticClass(), FoundEnemies);

		// プレイヤー以外の敵の消去
        for (AActor* EnemyActor : FoundEnemies)
        {
            if (EnemyActor != nullptr && EnemyActor != Player && EnemyActor != this)
            {
                EnemyActor->Destroy();
            }
        }

        UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());

        if (GI != nullptr)
        {
            GI->bIsBossDead = true;
            GI->bIsReturningFromBattle = true;
            GI->bIsBattleWon = true;

			// 倒したボスの名前を保存
            GI->DefeatedEnemyNames.AddUnique(this->BossUniqueName);
        }
    }

    return ActualDamage;
}



float AMyBossEnemy::GetHealthPercentage() const
{
    return (MaxHP > 0.0f) ? (CurrentHP / MaxHP) : 0.0f;
}



// ボスの次の攻撃タイプを決定する関数
EBossAttackType AMyBossEnemy::GetNextAttackType()
{
   
	// 大攻撃
    if (bIsCharging == true)
    {
        return EBossAttackType::Heavy;
    }

	// 溜め
    if (GetHealthPercentage() < 0.5f)
    {
        return EBossAttackType::StartCharge;
    }

	// それ以外の場合は通常攻撃
    return EBossAttackType::Normal;
}



// ボスの攻撃ダメージを計算する関数
float AMyBossEnemy::CalculateAttackDamage()
{
	// 溜め攻撃中は50、そうでなければ20のダメージ
    float BaseDamage = bIsCharging ? 50.0f : 20.0f;

	// HPが20%以下の場合は1.5倍のダメージ
    return (GetHealthPercentage() <= 0.2f) ? BaseDamage * 1.5f : BaseDamage;
}



// ボスの攻撃を実行する関数
void AMyBossEnemy::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
void AMyBossEnemy::CheckHealthStatus() {  }
void AMyBossEnemy::ExecuteSpecialAttack() {  }