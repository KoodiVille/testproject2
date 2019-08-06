// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueMonster.h"
#include "UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"
#include <Paper2D\Classes\PaperSpriteComponent.h>
#include <Engine.h>

class UPaperSpriteComponent;

// Sets default values
ABlueMonster::ABlueMonster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

ABlueMonster::ABlueMonster(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	CharacterSprite = ObjectInitializer.CreateDefaultSubobject<UPaperSpriteComponent>(this, TEXT("CharacterSprite"));
	CharacterSprite->SetEnableGravity(true);
	//CharacterSprite->SetMassScale(TEXT("Luuydin"), 2555.0f);
	CharacterSprite->SetupAttachment(RootComponent);
	//CharacterSprite->SetCollisionEnabled(true);
	CharacterSprite->SetMobility(EComponentMobility::Movable);
	//CharacterSprite->AddForce(FVector(0.f, 10.f, 0.f), TEXT("luu"), true);
	CharacterSprite->SetSimulatePhysics(true);

	CharacterSprite->SetNotifyRigidBodyCollision(true);
	CharacterSprite->BodyInstance.SetCollisionProfileName("BlockAll");

	RootComponent = CharacterSprite;

	AudioComponent = NULL;
}

void ABlueMonster::SetBounds()
{
	FHitResult OutHit;
	FVector Start = GetActorLocation() - FVector(0.f, 0.f, 150.f);
	FVector End = Start + FVector(0.f, 0.f, -Start.Z * 1000);
	FCollisionQueryParams CollisionParams;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic, CollisionParams)) {
		if (OutHit.bBlockingHit)
		{
			FVector orig;
			FVector ext;
			OutHit.GetActor()->GetActorBounds(true, orig, ext);
			RightEndLocation = new FVector(orig.X + ext.X - 20, orig.Y + ext.Y, orig.Z + ext.Z);
			LeftEndLocation = new FVector(orig.X - ext.X + 20.f, orig.Y - ext.Y, orig.Z - ext.Z);
		}
	}
}

void DebugLocation(ABlueMonster actor, FVector Start, FVector End, FVector orig, FVector ext, FHitResult OutHit)
{
	DrawDebugLine(actor.GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("left: %s"), *orig.ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("Right: %s"), *ext.ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("You are hitting: %s"), *OutHit.GetComponent()->GetName()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("You are hitting: %s"), *OutHit.GetActor()->GetName()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Impact Point: %s"), *OutHit.ImpactPoint.ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Normal Point: %s"), *OutHit.ImpactNormal.ToString()));
}

void ABlueMonster::Patrol(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	FRotator CurrentRotation = GetActorRotation();
	if (CurrentLocation.X >= RightEndLocation->X)
	{
		isGoingRight = false;
		CurrentRotation += FRotator(0.f, 180.f, 0.f);
	}
	else if (CurrentLocation.X <= LeftEndLocation->X)
	{
		isGoingRight = true;
		CurrentRotation -= FRotator(0.f, 180.f, 0.f);
	}

	if (isGoingRight)
	{
		CurrentLocation.X += 1.5f + DeltaTime;
	}
	else
	{
		CurrentLocation.X -= 1.5f + DeltaTime;
	}
	SetActorLocationAndRotation(CurrentLocation, CurrentRotation);
}

void ABlueMonster::OnOverLap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)  
{  
    // Other Actor is the actor that triggered the event. Check that is not ourself.  
    if ( (OtherActor != nullptr ) && (OtherActor != this) && ( OtherComp != nullptr ) )  
    {  
       if (GEngine) 
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("OnOverLap %s"),*OtherActor->GetName()));
		}
		UE_LOG(LogTemp, Verbose, TEXT("OnOverLap %s"), *OtherActor->GetName());
    }  
}


// Called when the game starts or when spawned
void ABlueMonster::BeginPlay()
{
	Super::BeginPlay();
	CharacterSprite->OnComponentHit.AddDynamic(this, &ABlueMonster::OnCompHit);
	CharacterSprite->OnComponentBeginOverlap.AddDynamic(this, &ABlueMonster::OnOverLap);
	StartLocation = GetActorLocation();
	isGoingRight = true;
	SetBounds();
}

// Called every frame
void ABlueMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Patrol(DeltaTime);
}

void ABlueMonster::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) 
{
	FString name = *OtherActor->GetName();
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && (name.Contains(TEXT("fak"))) )
	{
		if (GEngine) 
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit %s"),*OtherActor->GetName()));
		}

		// try and play the sound if specified
		if (HitSound != NULL)
		{
			if (AudioComponent != nullptr) {
				AudioComponent->Stop();
			}
			AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, HitSound, GetActorLocation());
		}

		UE_LOG(LogTemp, Verbose, TEXT("Hit %s"), *OtherActor->GetName());
	}
}
