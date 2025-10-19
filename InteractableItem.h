#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "InteractableItem.generated.h"

UCLASS()
class SPARTAPROJECT_API AInteractableItem : public ABaseItem
{
	GENERATED_BODY()

public:
    AInteractableItem();

	void OnInteract();

protected:
    UPROPERTY(VisibleAnywhere)
    class UWidgetComponent* InteractionWidget;

    virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime);

	virtual void OnItemOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
	virtual void OnItemEndOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;
};