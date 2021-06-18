// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlatformerCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "PlatformerGameMode.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);

//////////////////////////////////////////////////////////////////////////
// APlatformerCharacter

APlatformerCharacter::APlatformerCharacter()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	

	// Create an orthographic camera (no perspective) and attach it to the boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCameraComponent->OrthoWidth = 2048.0f;
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->SetUsingAbsoluteRotation(true);
	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->bAutoActivate = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;
}

void APlatformerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateCharacter();

}

void APlatformerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>();

	GetSprite()->OnComponentBeginOverlap.AddDynamic(this, &APlatformerCharacter::OnBeginOverlap);
}

void APlatformerCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (State == ECharacterState::Attack)
	{
		const float Damage = GetAttackDamage();

		UGameplayStatics::ApplyDamage(Other, Damage, GetController(), this, UDamageType::StaticClass());
	}
}

float APlatformerCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	Damage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp( Health - Damage, 0.0f, MaxHealth);

	if (Health == 0.0f)
	{
		OnDeath();
	}

	return Damage;
}

void APlatformerCharacter::OnDeath()
{
	State = ECharacterState::Dead;

	GetSprite()->SetFlipbook(DeathAnimation);
	GetSprite()->SetLooping(false);

	SetActorEnableCollision(false);

	SetCanBeDamaged(false);
}

void APlatformerCharacter::DestroyCharacter()
{
	Destroy();
}

void APlatformerCharacter::UpdateAnimation()
{
	if (State == ECharacterState::IdleMove)
	{
		const FVector PlayerVelocity = GetVelocity();
		const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

		UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;

		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void APlatformerCharacter::UpdateCharacter()
{
	UpdateAnimation();

	const float VelocityX = GetVelocity().X;

	if (VelocityX != 0.0f)
	{
		const float VelocityDirection = VelocityX > 0.0f ? 0.0f : 180.0f;

		if (Controller != nullptr)
		{
			Controller->SetControlRotation(FRotator(0.0, VelocityDirection, 0.0f));
		}
	}
}

void APlatformerCharacter::ResetState()
{
	State = ECharacterState::IdleMove;

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	GetSprite()->OnFinishedPlaying.Clear();

	GetSprite()->SetLooping(true);
	GetSprite()->Play();
}

bool APlatformerCharacter::PlayAttackAnimation(UPaperFlipbook* AttackAnimation)
{
	const bool bResult = AttackAnimation != nullptr && State == ECharacterState::IdleMove;

	if (bResult)
	{
		State = ECharacterState::Attack;

		GetSprite()->SetFlipbook(AttackAnimation);
		GetSprite()->SetLooping(false);
		GetSprite()->OnFinishedPlaying.AddDynamic(this, &APlatformerCharacter::ResetState);
	}

	return bResult;
}