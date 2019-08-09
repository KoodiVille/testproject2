// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TestProject2Character.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "AGun.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Components/TimelineComponent.h"

class UAudioComponent;

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);

//////////////////////////////////////////////////////////////////////////
// ATestProject2Character

ATestProject2Character::ATestProject2Character()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);


	// Create a gun mesh component
	PlayerGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerGun"));
	PlayerGun->SetOnlyOwnerSee(false);			// only the owning player will see this mesh
	PlayerGun->bCastDynamicShadow = false;
	PlayerGun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	PlayerGun->SetupAttachment(RootComponent);


	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->bAbsoluteRotation = true;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f);
	

	// Create an orthographic camera (no perspective) and attach it to the boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCameraComponent->OrthoWidth = 2048.0f;
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->bAbsoluteRotation = true;
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

    // 	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
    // 	TextComponent->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));
    // 	TextComponent->SetRelativeLocation(FVector(35.0f, 5.0f, 20.0f));
    // 	TextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    // 	TextComponent->SetupAttachment(RootComponent);

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);

	AudioComponent = NULL;
}

void ATestProject2Character::BeginPlay() 
{
	Super::BeginPlay();

	FullHealth = 1000.0f;
	Health = FullHealth;
	HealthPercentage = 1.0f;
	bCanBeDamaged = true;

	FullGunSpeed = 100.0f;
	GunSpeed = FullGunSpeed;
	GunSpeedPercentage = 1.0f;
	PreviousGunSpeed = GunSpeedPercentage;
	GunSpeedValue = 0.0f;
	bCanUseGun = true;

	if (GunSpeedCurve)
    {
        FOnTimelineFloat TimelineCallback;
        FOnTimelineEventStatic TimelineFinishedCallback;

        TimelineCallback.BindUFunction(this, FName("SetGunSpeedValue"));
        TimelineFinishedCallback.BindUFunction(this, FName("SetGunSpeedState"));

		MyTimeline = NewObject<UTimelineComponent>(this, FName("Gun Speed Animation"));
        MyTimeline->AddInterpFloat(GunSpeedCurve, TimelineCallback);
        MyTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
		MyTimeline->RegisterComponent();
    }
}

//////////////////////////////////////////////////////////////////////////
// Animation

void ATestProject2Character::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	// Are we moving or standing still?
	UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;
	if( GetSprite()->GetFlipbook() != DesiredAnimation 	)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void ATestProject2Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateCharacter();	
}


void ATestProject2Character::Shoot()
{
	// try and fire a projectile
	if (ProjectileClass != NULL && !FMath::IsNearlyZero(GunSpeed, 0.001f) && bCanUseGun)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr)
				? FP_MuzzleLocation->GetComponentLocation()
				: GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			
			FVector supalocation;
			if (GetVelocity().X < 0.f)
			{
				supalocation = GetActorLocation() + FVector(-75.f, 0.f, 0.f);
			} 
			else if(GetVelocity().X > 0.f)
			{
				supalocation = GetActorLocation() + FVector(75.f, 0.f, 0.f);
			}
			else
			{
				supalocation = SpawnLocation;
			}

			// spawn the projectile at the muzzle
			World->SpawnActor<ABullet>(ProjectileClass, supalocation, SpawnRotation, ActorSpawnParams);

		}

		// try and play the sound if specified
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}

		if(MyTimeline != nullptr) MyTimeline->Stop();
		GetWorldTimerManager().ClearTimer(GunSpeedTimerHandle);
		SetGunSpeedChange(-20.0f);
		GetWorldTimerManager().SetTimer(GunSpeedTimerHandle, this, &ATestProject2Character::UpdateGunSpeed, 5.0f, false);
	}
}


//////////////////////////////////////////////////////////////////////////
// Input

void ATestProject2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATestProject2Character::Shoot);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATestProject2Character::MoveRight);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATestProject2Character::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATestProject2Character::TouchStopped);
}

void ATestProject2Character::MoveRight(float Value)
{
	/*UpdateChar();*/

	// Apply the input to the character motion
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void ATestProject2Character::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Jump on any touch
	Jump();
}

void ATestProject2Character::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Cease jumping once touch stopped
	StopJumping();
}

void ATestProject2Character::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();	
	float TravelDirection = PlayerVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}

float ATestProject2Character::GetHealth()
{
	return HealthPercentage;
}

float ATestProject2Character::GetGunSpeed()
{
	return GunSpeedPercentage;
}

FText ATestProject2Character::GetHealthIntText()
{
	int32 HP = FMath::RoundHalfFromZero(HealthPercentage * 100);
	FString HPS = FString::FromInt(HP);
	FString HealthHUD = HPS + FString(TEXT("%"));
	FText HPText = FText::FromString(HealthHUD);
	return HPText;
}

void ATestProject2Character::SetDamageState()
{
	bCanBeDamaged = true;
}

void ATestProject2Character::DamageTimer()
{
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &ATestProject2Character::SetDamageState, 2.0f, false);
}

void ATestProject2Character::SetGunSpeedValue()
{
	TimelineValue = MyTimeline->GetPlaybackPosition();
    CurveFloatValue = PreviousGunSpeed + GunSpeedValue*GunSpeedCurve->GetFloatValue(TimelineValue);
	GunSpeed = FMath::Clamp(CurveFloatValue*FullHealth, 0.0f, FullGunSpeed);
	GunSpeedPercentage = FMath::Clamp(CurveFloatValue, 0.0f, 1.0f);
}

void ATestProject2Character::SetGunSpeedState()
{
	bCanUseGun = true;
	GunSpeedValue = 0.0;
}

bool ATestProject2Character::PlayFlash()
{
	if(redFlash)
	{
		redFlash = false;
		return true;
	}

	return false;
}

float ATestProject2Character::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	bCanBeDamaged = false;
	redFlash = true;
	UpdateHealth(-DamageAmount);
	DamageTimer();
	// try and play the sound if specified
	if (HitSound != NULL)
	{
		if (AudioComponent != nullptr) {
			AudioComponent->Stop();
		}
		AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, HitSound, GetActorLocation());
	}
	return DamageAmount;
}

void ATestProject2Character::UpdateHealth(float HealthChange)
{
	Health = FMath::Clamp(Health += HealthChange, 0.0f, FullHealth);
	HealthPercentage = Health/FullHealth;
}

void ATestProject2Character::UpdateGunSpeed()
{
	PreviousGunSpeed = GunSpeedPercentage;
	GunSpeedPercentage = GunSpeed/FullGunSpeed;
	GunSpeedValue = 1.0f;
	if(MyTimeline != nullptr) MyTimeline->PlayFromStart();
}

void ATestProject2Character::SetGunSpeedChange(float GunSpeedChange)
{
	bCanUseGun = false;
	PreviousGunSpeed = GunSpeedPercentage;
	GunSpeedValue = (GunSpeedChange/FullGunSpeed);

	if(MyTimeline != nullptr) MyTimeline->PlayFromStart();
}
