#include "InteractableItem.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

AInteractableItem::AInteractableItem()
{
    PrimaryActorTick.bCanEverTick = true;

    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(RootComponent);
    InteractionWidget->SetWidgetSpace(EWidgetSpace::World);
    InteractionWidget->SetDrawSize(FVector2D(300.f, 100.f));
    InteractionWidget->SetRelativeLocation(FVector(0, 0, 100.f));
}

void AInteractableItem::OnInteract()
{
    Destroy();
}

void AInteractableItem::BeginPlay()
{
    Super::BeginPlay();
    InteractionWidget->SetVisibility(false);
}

void AInteractableItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AInteractableItem::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    InteractionWidget->SetVisibility(true);
}

void AInteractableItem::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    InteractionWidget->SetVisibility(false);
}