#include "CoinItem.h"
#include "SpartaGameState.h"
#include "Engine/World.h"

ACoinItem::ACoinItem()
	: SpinSpeed(180.f)
	, MoveSpeed(200.f)
	, MoveDistance(200.f)
	, Direction(1.0f)
	, bMovingMode(false)
	, PointValue(10)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACoinItem::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
}

void ACoinItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSpin(DeltaTime);

	if (bMovingMode)
	{
		Move(DeltaTime);
	}
}

void ACoinItem::UpdateSpin(float DeltaTime)
{
	// 코인을 Yaw축 기준으로 회전시킴
	AddActorLocalRotation(FRotator(0.f, SpinSpeed * DeltaTime, 0.f));
}

void ACoinItem::Move(float DeltaTime)
{
	float CurrentDistance = (GetActorLocation() - StartLocation).Length();

	if (MoveDistance <= CurrentDistance)
	{
		StartLocation = GetActorLocation();
		Direction *= -1.0f;
	}

	FVector DeltaLocation = GetActorUpVector() * Direction * MoveSpeed * DeltaTime;
	AddActorLocalOffset(DeltaLocation);
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	// 플레이어가 코인을 획득한 경우
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (ASpartaGameState* GameState = World->GetGameState<ASpartaGameState>())
			{
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected();
			}
		}
		DestroyItem();
	}
}

void ACoinItem::SetWave3MoveActive(bool bActive)
{
	bMovingMode = bActive;
}
