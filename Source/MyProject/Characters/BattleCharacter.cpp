#include "BattleCharacter.h" 
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h" 
#include "Engine/World.h"
#include "Logging/LogMacros.h"
#include "Engine/Engine.h"
#include "MyProject/Core/MyGameInstance.h"



ABattleCharacter::ABattleCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

	//  カメラのセットアップ
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// 初期ステータスの設定
    CurrentLevel = 1;
    CurrentExp = 0;
    MaxHP = 150.0f;
    CurrentHP = MaxHP;
    AttackDamage = 10.0f;

	// ターン制バトルのためのフラグ
    bIsPlayerTurn = true;
}



void ABattleCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());

	// ThirdPersonCharacterのインスタンスである場合のみ、GameInstanceからの引き継ぎ
    if (GI != nullptr && GetName().Contains(TEXT("ThirdPersonCharacter")))
    {
		// GameInstanceに保存されているステータスがある場合は、引き継ぎ
        if (GI->StoredLevel > 0)
        {
            CurrentLevel = GI->StoredLevel;
            MaxHP = GI->StoredMaxHP;
            CurrentHP = GI->StoredHP;
            AttackDamage = GI->StoredAttackDamage;
            CurrentExp = GI->StoredCurrentExp;
        }

        else
        {
            GI->StoredLevel = CurrentLevel;
            GI->StoredHP = CurrentHP;
            GI->StoredMaxHP = MaxHP;
            GI->StoredAttackDamage = AttackDamage;
            GI->StoredCurrentExp = CurrentExp;
        }
    }
}



void ABattleCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}



// プレイヤーの入力をバインド
void ABattleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxisKey(EKeys::W, this, &ABattleCharacter::MoveForward);
    PlayerInputComponent->BindAxisKey(EKeys::S, this, &ABattleCharacter::MoveForward_Back);
    PlayerInputComponent->BindAxisKey(EKeys::D, this, &ABattleCharacter::MoveRight);
    PlayerInputComponent->BindAxisKey(EKeys::A, this, &ABattleCharacter::MoveRight_Left);

    PlayerInputComponent->BindAxisKey(EKeys::MouseX, this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxisKey(EKeys::MouseY, this, &ABattleCharacter::LookUp);

    PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ABattleCharacter::Attack);
}



void ABattleCharacter::MoveForward(float Value)
{
	
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}



void ABattleCharacter::MoveRight(float Value)
{

    if ((Controller != nullptr) && (Value != 0.0f))
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}



void ABattleCharacter::Attack()
{

    if (GetController() == nullptr)
    {
        return;
    }


    if (!GetController()->IsPlayerController())
    {
        return;
    }

	// 攻撃中であれば、攻撃をキャンセル
    if (bIsAttacking != false) 
    {
        return;
    }

	// 攻撃中のフラグをオンにする
    bIsAttacking = true;

	// 攻撃アニメーションを再生
    if (AttackMontage != nullptr)
    {

        float Duration = PlayAnimMontage(AttackMontage);

    }

	// 攻撃が終了したら、攻撃中のフラグをオフにする
    bIsAttacking = false;
}



void ABattleCharacter::LookUp(float Value)
{
    AddControllerPitchInput(-Value);
}



void ABattleCharacter::MoveForward_Back(float Value)
{
    MoveForward(-Value);
}



void ABattleCharacter::MoveRight_Left(float Value)
{
    MoveRight(-Value);
}



float ABattleCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{

    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// ガードしている場合は、ダメージを半減
    if (bIsGuarding == true)
    {
        ActualDamage = ActualDamage * 0.5f;

        if (GEngine != nullptr)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Guarded! Damage Reduced."));
        }
    }

	// ガードが終了したら、ガードフラグをオフにする
    bIsGuarding = false;

	// ダメージを受けた後のHPを計算
    CurrentHP = FMath::Clamp(CurrentHP - ActualDamage, 0.0f, MaxHP);

    UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());

	// プレイヤーのHPをGameInstanceに保存
    if (GI != nullptr)
    {
        if (GetController() != nullptr && GetController()->IsPlayerController())
        {
            GI->StoredHP = CurrentHP;
        }
    }

	// 死亡判定を行う
    if (CurrentHP <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Character is Dead!"));

		// プレイヤーが死んだときは、GameOverレベルに遷移する
        if (GetController() != nullptr && GetController()->IsPlayerController())
        {
			// プレイヤーが死んだときは、GameInstanceのステータスをリセット
            if (GI != nullptr)
            {
                GI->ResetPlayerStats();
            }

            UGameplayStatics::OpenLevel(this, FName("L_GameOver"));
        }

        else
        {
            if (GEngine != nullptr)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Enemy Defeated!"));
            }
        }
    }

    return ActualDamage;
}



void ABattleCharacter::GainExp(int32 Amount)
{
	// 最大レベルに達しているかどうかの確認
    if (CurrentLevel >= MaxLevel)
    {

        CurrentLevel = MaxLevel;
        CurrentExp = 0;

        UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());

        if (GI != nullptr)
        {
            GI->StoredHP = CurrentHP;
            GI->StoredLevel = CurrentLevel;
            GI->StoredCurrentExp = CurrentExp;
            GI->StoredMaxHP = MaxHP;
            GI->StoredAttackDamage = AttackDamage;
        }

        return;
    }

    CurrentExp += Amount;

	// レベルアップの条件を満たしている場合は、レベルアップ
    while (CurrentLevel < MaxLevel && CurrentExp >= (CurrentLevel * 5))
    {
        CurrentExp -= (CurrentLevel * 5);
        
        LevelUp(); 
    }

    UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());

    if (GI != nullptr)
    {
        GI->StoredLevel = CurrentLevel;
        GI->StoredCurrentExp = CurrentExp;
        GI->StoredMaxHP = MaxHP;
        GI->StoredHP = CurrentHP;
        GI->StoredAttackDamage = AttackDamage;

    }
}



void ABattleCharacter::LevelUp()
{
    CurrentLevel++;
    MaxHP += 20.0f;
    AttackDamage += 5.0f;
    CurrentHP = MaxHP;

    if (GEngine != nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT(" LEVEL UP! Now Level %d "), CurrentLevel));
    }
}



void ABattleCharacter::SetGuarding(bool bNewGuarding)
{
	// ガードフラグを更新
    bIsGuarding = bNewGuarding;

    if ((GEngine != nullptr) && (bIsGuarding == true))
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Guard"));
    }
}