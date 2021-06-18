#include "EnemyCharacter.h"
#include "PlatformerGameMode.h"
#include "PaperFlipbookComponent.h"
#include "PlayerCharacter.h"

void AEnemyCharacter::OnDeath()
{
	Super::OnDeath();

	GameMode->OnEnemyDeath();

	GetSprite()->OnFinishedPlaying.AddDynamic(this, &AEnemyCharacter::DestroyCharacter);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyCharacter::Attack()
{
	if (Player != nullptr && !Player->IsInState(ECharacterState::Dead))
	{
		PlayAttackAnimation(AttackAnimation);
	}
}