// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "AGun.h"
#include "TestProject2Character.generated.h"

class UTextRenderComponent;
class UTimelineComponent;
class AGun;

/**
 * This class is the default character for TestProject2, and it is responsible for all
 * physical interaction between the player and the world.
 *
 * The capsule component (inherited from ACharacter) handles collision with the world
 * The CharacterMovementComponent (inherited from ACharacter) handles movement of the collision capsule
 * The Sprite component (inherited from APaperCharacter) handles the visuals
 */
UCLASS(config = Game)
class ATestProject2Character : public APaperCharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent *SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent *CameraBoom;

	UTextRenderComponent *TextComponent;

	virtual void Tick(float DeltaSeconds) override;

protected:
	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook *RunningAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook *IdleAnimation;

	UPROPERTY(VisibleDefaultsOnly, Category = "Gun")
	class USkeletalMeshComponent *PlayerGun;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent *Mesh1P;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent *FP_MuzzleLocation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase *FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage *FireAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class ABullet> ProjectileClass;

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	/** Called for side to side input */
	void MoveRight(float Value);

	void UpdateCharacter();

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent *InputComponent) override;
	// End of APawn interface

	void Shoot();

public:
	ATestProject2Character();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent *GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return CameraBoom; }

	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float FullHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunSpeed")
	float GunSpeedPercentage;

 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float FullGunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunSpeed")
	float GunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunSpeed")
	float PreviousGunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunSpeed")
	float GunSpeedValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool redFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunSpeed")
	UCurveFloat* GunSpeedCurve;

	float CurveFloatValue;
	float TimelineValue;
	bool bCanUseGun;

	UTimelineComponent* MyTimeline;
	struct FTimerHandle MemberTimerHandle;
	struct FTimerHandle GunSpeedTimerHandle;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth();

	UFUNCTION(BlueprintPure, Category = "GunSpeed")
	float GetGunSpeed();

	UFUNCTION(BlueprintPure, Category = "Health")
	FText GetHealthIntText();

	UFUNCTION()
	void DamageTimer();

	UFUNCTION()
	void SetDamageState();

	UFUNCTION()
	void SetGunSpeedValue();

	UFUNCTION()
	void SetGunSpeedState();

	UFUNCTION()
	void SetGunSpeedChange(float GunSpeedChange);

	UFUNCTION()
	void UpdateGunSpeed();

	UFUNCTION(BlueprintPure, Category = "Health")
	bool PlayFlash();

	UFUNCTION(BlueprintCallable, Category = "Health")
	void UpdateHealth(float HealthChange);

	/** Sound to play each time we get hit by monster */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* HitSound;

	UAudioComponent* AudioComponent;
};
