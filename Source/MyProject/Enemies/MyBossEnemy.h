#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h" 
#include "MyProject/Characters/BattleCharacter.h"
#include "MyBossEnemy.generated.h"

UENUM(BlueprintType)
enum class EBossAttackType : uint8
{
    Normal      UMETA(DisplayName = "Normal Attack"),
    StartCharge UMETA(DisplayName = "Start Charging"),
    Heavy       UMETA(DisplayName = "Heavy Attack")
};

UCLASS()
class MYPROJECT_API AMyBossEnemy : public ABattleCharacter
{
    GENERATED_BODY()

public:
    AMyBossEnemy();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossSettings")
    FName BossUniqueName = FName("FinalBoss_01");

private:
    double LastDamageTime = 0.0;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
    UBoxComponent* BattleTrigger;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossStats")
    int32 GiveExp = 60;

    UPROPERTY(BlueprintReadWrite, Category = "BossStats")
    bool bIsCharging = false;

    UFUNCTION(BlueprintPure, Category = "BossStats")
    float GetHealthPercentage() const;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(BlueprintCallable, Category = "BossLogic")
    EBossAttackType GetNextAttackType();

    UFUNCTION(BlueprintCallable, Category = "BossLogic")
    float CalculateAttackDamage();

    UFUNCTION(BlueprintCallable, Category = "BossLogic")
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION(BlueprintCallable, Category = "BossLogic")
    void CheckHealthStatus();

    UFUNCTION(BlueprintCallable, Category = "BossLogic")
    void ExecuteSpecialAttack();
};