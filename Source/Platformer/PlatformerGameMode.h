// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlatformerGameMode.generated.h"

class APlayerCharacter;
class UPlatformerGameInstance;

UCLASS(minimalapi)
class APlatformerGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Platformer Game Mode")
	int32 MaxLevelNumber = 3;

	int32 EnemyCount;

	UPROPERTY()
	APlayerCharacter* Player;

	UPROPERTY()
	UPlatformerGameInstance* GameInstance;

	APlatformerGameMode();

	virtual void BeginPlay() override;

	void GoToNextLevel() const;

public:

  void OnEnemyDeath();

  void SetPlayer(APlayerCharacter* InPlayer) { Player = InPlayer; }

  void RestartLevel();
};
