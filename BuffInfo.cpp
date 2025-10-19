#include "BuffInfo.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UBuffInfo::NativeConstruct()
{
    Super::NativeConstruct();

    // 초기값 설정
    TotalDuration = 0.0f;
    RemainingTime = 0.0f;

    if (BuffNameText)
    {
        BuffNameText->SetText(FText::FromString(TEXT("")));
    }

    if (BuffDurationBar)
    {
        BuffDurationBar->SetPercent(1.0f);
    }
}

void UBuffInfo::InitializeBuffInfo(const FString& InBuffName, float InDuration)
{
    TotalDuration = InDuration;
    RemainingTime = InDuration;

    if (BuffNameText)
    {
        BuffNameText->SetText(FText::FromString(InBuffName));
    }

    if (BuffDurationBar)
    {
        BuffDurationBar->SetPercent(1.0f);
    }
}

void UBuffInfo::ExtendBuffDuration(float AdditionalTime)
{
    TotalDuration += AdditionalTime;
    RemainingTime = FMath::Max(RemainingTime + AdditionalTime, AdditionalTime);

    UpdateBuffUI();
}

const FString& UBuffInfo::GetBuffName() const
{
    return BuffNameText->GetText().ToString();
}

void UBuffInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (RemainingTime > 0.0f)
    {
        RemainingTime -= InDeltaTime;
        UpdateBuffUI();

        // 남은 시간이 0보다 작다면 제거
        if (RemainingTime <= 0.0f)
        {
            RemoveFromParent();
        }
    }
}

void UBuffInfo::UpdateBuffUI()
{
    if (BuffDurationBar && TotalDuration > 0.0f)
    {
        BuffDurationBar->SetPercent(RemainingTime / TotalDuration);
    }
}
