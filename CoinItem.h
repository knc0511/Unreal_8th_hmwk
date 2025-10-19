#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "CoinItem.generated.h"

UCLASS()
class SPARTAPROJECT_API ACoinItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ACoinItem();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void ActivateItem(AActor* Activator) override;

public:
	// Wave 3 시작 시, 이 함수를 호출하여 움직임 모드를 On/Off
	void SetWave3MoveActive(bool bActive);

protected:
	// 코인의 제자리에서 회전하는 속도 (deg/sec)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float SpinSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float MoveDistance;

	float Direction;
	FVector StartLocation;
	
	// Wave 3 시 무브 모드 활성화 여부
	bool bMovingMode;

	// 코인을 획득했을 때 증가하는 점수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	int32 PointValue;

private:
	// 코인의 자전(Spin) 업데이트 함수
	void UpdateSpin(float DeltaTime);

	// 이동 업데이트 함수
	void Move(float DeltaTime);
};
