#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "UObjectGlobals.h"
#include "ABullet.h"
#include "AGun.generated.h"

UCLASS(config = Game)
class AGun : public APawn
{
	GENERATED_BODY()

	float cooldown;

public:
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

	AGun(const class FObjectInitializer &ObjectInitializer);
	void Shoot();
	void Reload();
};
