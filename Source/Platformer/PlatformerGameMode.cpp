// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlatformerGameMode.h"
#include "PlatformerCharacter.h"
#include "Platformer.h"
#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "PlayerCharacter.h"
#include "PlatformerGameInstance.h"

APlatformerGameMode::APlatformerGameMode()
{
	DefaultPawnClass = APlatformerCharacter::StaticClass();
}

void APlatformerGameMode::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UPlatformerGameInstance>(GetGameInstance());

	// Get how many enemies are in the level

	for (TActorIterator<AEnemyCharacter> EnemyItr(GetWorld()); EnemyItr; ++EnemyItr)
	{
		EnemyCount++;
	}
}
void APlatformerGameMode::OnEnemyDeath()
{
	EnemyCount--;

	GameInstance->AddScore(25);

	if (EnemyCount == 0)
	{
		const float NewPlayerHealth = Player->GetNextLevelHealth();

		GameInstance->SetHealth(NewPlayerHealth);

		GoToNextLevel();
	}
}

void APlatformerGameMode::RestartLevel()
{
	const FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this]()
	{
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	});

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 2.0f, false);
}

void APlatformerGameMode::GoToNextLevel() const
{
	const FString CurrentLevel = GetWorld()->GetName();
	const FString StringLevelNumber = CurrentLevel.RightChop(6);

	const int32 LevelNumber = FCString::Atoi(*StringLevelNumber);
	const int32 NextLevelNumber = LevelNumber + 1;

	const FString NextLevel = NextLevelNumber <= MaxLevelNumber ? "Level_" + FString::FromInt(NextLevelNumber) : "Menu";

	const FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this, NextLevel]()
	{
		UGameplayStatics::OpenLevel(this, FName(*NextLevel), false);
	});

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 2.0f, false);
}