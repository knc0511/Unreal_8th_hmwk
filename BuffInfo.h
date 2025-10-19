// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuffInfo.generated.h"

class UTextBlock;
class UProgressBar;

// 버프 정보를 표시하는 위젯 (버프 이름 + 지속시간)
UCLASS()
class SPARTAPROJECT_API UBuffInfo : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Buff")
    void InitializeBuffInfo(const FString& InBuffName, float InDuration);

    UFUNCTION(BlueprintCallable, Category = "Buff")
    void ExtendBuffDuration(float AdditionalTime);

    const FString& GetBuffName() const;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    void UpdateBuffUI();

private:
    // 버프 이름
    UPROPERTY(meta = (BindWidget))
    UTextBlock* BuffNameText;

    // 버프 남은 시간 표시용 프로그레스바
    UPROPERTY(meta = (BindWidget))
    UProgressBar* BuffDurationBar;

    // 버프 지속 시간
    float TotalDuration;

    // 버프 남은 시간
    float RemainingTime;
};