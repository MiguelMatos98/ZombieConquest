#include "PlayerCharacter.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Platformer.h"
#include "PlatformerGameMode.h"
#include "PaperSpriteComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlatformerGameInstance.h"

APlayerCharacter::APlayerCharacter()
{
    BlockSprite = CreateDefaultSubobject<UPaperSpriteComponent>("Block Sprite");
    BlockSprite->SetupAttachment(RootComponent);

    GetCharacterMovement()->JumpZVelocity = 1200.0f;
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    GameMode->SetPlayer(this);

    UPlatformerGameInstance* GameInstance = Cast<UPlatformerGameInstance>(GetGameInstance());

    if (GameInstance != nullptr)
    {
        const float PreviousLevelHealth = GameInstance->GetHealth();

        if (PreviousLevelHealth != -1.0f)
        {
            Health = PreviousLevelHealth;
        }
    }

    SetIsBlocking(false);
}

float APlayerCharacter::GetAttackDamage() const
{
    float Damage = 0.0f;

    if (GetSprite()->GetFlipbook() == LightAttackAnimation)
    {
        Damage = LightAttackDamage;
    }
    else if (GetSprite()->GetFlipbook() == HeavyAttackAnimation)
    {
        Damage = HeavyAttackDamage;
    }

    return Damage;
}

void APlayerCharacter::OnDeath()
{
    Super::OnDeath();

    SetIsBlocking(false);

    GameMode->RestartLevel();
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("Light Attack", IE_Pressed, this, &APlayerCharacter::OnPressedLightAttack);
    PlayerInputComponent->BindAction("Heavy Attack", IE_Pressed, this, &APlayerCharacter::OnPressedHeavyAttack);
    PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerCharacter::OnPressedDash);
    PlayerInputComponent->BindAction("Block", IE_Pressed, this, &APlayerCharacter::OnPressedBlock);
    PlayerInputComponent->BindAction("Block", IE_Released, this, &APlayerCharacter::OnReleasedBlock);

    PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::OnAxisMoveRight);
}

void APlayerCharacter::OnAxisMoveRight(float Value)
{
    // Apply the input to the character motion
    AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void APlayerCharacter::OnPressedLightAttack() 
{
    if (PlayAttackAnimation(LightAttackAnimation))
    {
        SetIsBlocking(false);
    }
}

void APlayerCharacter::OnPressedHeavyAttack()
{
    if (PlayAttackAnimation(HeavyAttackAnimation))
    {
        SetIsBlocking(false);
    }
}

void APlayerCharacter::OnPressedDash()
{
    if (!GetWorldTimerManager().IsTimerActive(DashCooldown))
    {
        LaunchCharacter(GetActorForwardVector() * DashForce, true, false);

        GetWorldTimerManager().SetTimer(DashCooldown, DashCooldownDuration, false);
    }
}

void APlayerCharacter::OnPressedBlock()
{
    if(State == ECharacterState::IdleMove)
    {
        SetIsBlocking(true);
    }
}

void APlayerCharacter::OnReleasedBlock()
{
    SetIsBlocking(false);
}

void APlayerCharacter::SetIsBlocking(bool bIsBlocking)
{
    SetCanBeDamaged(!bIsBlocking);

    BlockSprite->SetVisibility(bIsBlocking);
}

float APlayerCharacter::GetNextLevelHealth()
{
    return FMath::Clamp(Health + 50.0f, 0.0f, MaxHealth);
}