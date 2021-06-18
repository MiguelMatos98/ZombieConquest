#pragma once

#include "CoreMinimal.h"
#include "PlatformerCharacter.h"
#include "PlayerCharacter.generated.h"

class UPaperSpriteComponent;

UCLASS()
class PLATFORMER_API APlayerCharacter : public APlatformerCharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Character")
	UPaperSpriteComponent* BlockSprite;

	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	float DashForce = 2500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	float DashCooldownDuration = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	UPaperFlipbook* LightAttackAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	UPaperFlipbook* HeavyAttackAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	float LightAttackDamage = 25.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	float HeavyAttackDamage = 50.0f;

	FTimerHandle DashCooldown;

	APlayerCharacter();

	virtual void BeginPlay() override;

	virtual void OnDeath() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnAxisMoveRight(float Value);

	void OnPressedLightAttack();

	void OnPressedHeavyAttack();

	void OnPressedDash();

	void OnPressedBlock();
	void OnReleasedBlock();

	void SetIsBlocking(bool bIsBlocking);

	virtual float GetAttackDamage() const override;

public:

	float GetNextLevelHealth();
};
