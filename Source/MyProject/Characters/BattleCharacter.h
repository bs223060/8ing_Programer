
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
class UMyGameInstance;
#include "BattleCharacter.generated.h"

UCLASS()
class MYPROJECT_API ABattleCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABattleCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveForward_Back(float Value);
	void MoveRight_Left(float Value);
	void LookUp(float Value);

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Attack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange = 300.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Status")
	float CurrentHP;

	UPROPERTY(BlueprintReadWrite, Category = "Status")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 CurrentExp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	bool bIsGuarding = false;

	UFUNCTION(BlueprintCallable, Category = "Status")
	void GainExp(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetGuarding(bool bNewGuarding);

	UFUNCTION(BlueprintCallable, Category = "Status")
	void LevelUp();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 CurrentLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 MaxLevel = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 MaxExp = 100;

	// エディタから攻撃アニメーションを選択できるようにする
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* AttackMontage;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsAttacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleSystem")
	bool bIsPlayerTurn = true;

};
