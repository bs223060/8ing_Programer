#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ResultWidget.generated.h"

UCLASS()
class MYPROJECT_API UResultWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// Widget内のパーツとC++を紐付ける（名前をWidget側の配置と一致させます）
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ExpProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ExpText;

	// アニメーション用の変数
	float CurrentExpDisplay;
	float TargetExpDisplay;
	float MaxExpDisplay;
	float AnimationSpeed;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	// リザルト開始時に数値をセットする関数
	UFUNCTION(BlueprintCallable, Category = "Result")
	void SetupResultValues(float CurrentExp, float EarnedExp, float MaxExp);
};