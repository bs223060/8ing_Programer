#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "MyProject/Characters/BattleCharacter.h"
#include "Enemy_Zako.generated.h"

UCLASS()
class MYPROJECT_API AEnemy_Zako : public ABattleCharacter
{
	GENERATED_BODY()

public:
	AEnemy_Zako();

protected:
	virtual void BeginPlay() override;

	void CheckAndDestroySelf();

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	USphereComponent* BattleTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	int32 EnemyID = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	FName EnemyUniqueName;

	UFUNCTION(BlueprintPure, Category = "Battle")
	float GetHealthPercentage() const;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};