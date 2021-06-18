#pragma once

#include "CoreMinimal.h"
#include "PlatformerCharacter.h"
#include "EnemyCharacter.generated.h"

class APlayerCharacter;

UCLASS()
class PLATFORMER_API AEnemyCharacter : public APlatformerCharacter
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Character")
	UPaperFlipbook* AttackAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Character")
	float AttackDamage = 10.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Enemy Character")
	APlayerCharacter* Player;

	UFUNCTION(BlueprintCallable, Category = "Enemy Character")
	void Attack();

	virtual void OnDeath() override;
	virtual void BeginPlay() override;

	virtual float GetAttackDamage() const override { return AttackDamage; };
};
