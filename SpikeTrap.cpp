#include "SpikeTrap.h"
#include "Components/StaticMeshComponent.h"

ASpikeTrap::ASpikeTrap()
	: RiseHeight(50.f)
	, RiseDuration(1.f)
	, bIsActivating(false)
	, CurrentTime(0.f)
{
	PrimaryActorTick.bCanEverTick = true;

	SpikeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpikeMesh"));
	SetRootComponent(SpikeMesh);
}

void ASpikeTrap::BeginPlay()
{
	Super::BeginPlay();

	DefaultRelativeLocation = SpikeMesh->GetRelativeLocation();

	FVector StartLoc = DefaultRelativeLocation;
	StartLoc.Z -= RiseHeight;
	SpikeMesh->SetRelativeLocation(StartLoc);
}

void ASpikeTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActivating)
	{
		UpdateSpike(DeltaTime);
	}
}

void ASpikeTrap::UpdateSpike(float DeltaTime)
{
	CurrentTime += DeltaTime;
	const float Alpha = FMath::Clamp(CurrentTime / RiseDuration, 0.f, 1.f);
	const float NewZ = FMath::Lerp(DefaultRelativeLocation.Z - RiseHeight, DefaultRelativeLocation.Z, Alpha);
	FVector CurrentLoc = SpikeMesh->GetRelativeLocation();
	CurrentLoc.Z = NewZ;
	SpikeMesh->SetRelativeLocation(CurrentLoc);

	if (Alpha >= 1.f)
	{
		bIsActivating = false;
	}
}

void ASpikeTrap::ActivateTrap()
{
	bIsActivating = true;
	CurrentTime = 0.f;
}
