#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

class ASpawnVolume;
class ACoinItem;
class ASpikeTrap;
class ASpartaPlayerController;
class USpartaGameInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveNumber);

UCLASS()
class SPARTAPROJECT_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()

public:
	ASpartaGameState();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWaves;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	float WaveDuration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<int32> ItemsToSpawnPerWave;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave|Hazard")
	TSubclassOf<AActor> SpikeTrapClass;

	FTimerHandle WaveTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

	void StartLevel();
	void EndLevel();
	void StartWave();
	void EndWave();
	void OnWaveTimeUp();
	void OnCoinCollected();
	void UpdateHUD();

private:
	UPROPERTY()
	TArray<AActor*> CurrentWaveItems;

	void EnableWave2();
	void EnableWave3();
	void SetAllCoinsMove(bool bActive);

	ASpawnVolume* GetSpawnVolume() const;
	ASpartaPlayerController* GetSpartaPlayerController() const;
	USpartaGameInstance* GetSpartaGameInstance() const;
};