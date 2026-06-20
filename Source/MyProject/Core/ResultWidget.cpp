#include "ResultWidget.h"

void UResultWidget::NativeConstruct()
{
	Super::NativeConstruct();
	AnimationSpeed = 1.5f; // バーが動く速度（好みに合わせて調整）
}

void UResultWidget::SetupResultValues(float CurrentExp, float EarnedExp, float MaxExp)
{
	CurrentExpDisplay = CurrentExp;
	TargetExpDisplay = CurrentExp + EarnedExp;
	MaxExpDisplay = MaxExp;

	// テキストに「+15 EXP」のように表示
	if (ExpText)
	{
		ExpText->SetText(FText::FromString(FString::Printf(TEXT("+%.0f EXP"), EarnedExp)));
	}
}

// 毎フレーム呼ばれる（UIのTick）
void UResultWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 現在の表示用Expが、目標のExpに近づくようにじわじわ補間計算する
	if (CurrentExpDisplay < TargetExpDisplay)
	{
		CurrentExpDisplay = FMath::FInterpTo(CurrentExpDisplay, TargetExpDisplay, InDeltaTime, AnimationSpeed);

		// 進捗バーに％を反映（0.0 ～ 1.0 の値にするため MaxExp で割る）
		if (ExpProgressBar && MaxExpDisplay > 0.0f)
		{
			float Percent = CurrentExpDisplay / MaxExpDisplay;
			ExpProgressBar->SetPercent(Percent);
		}
	}
}