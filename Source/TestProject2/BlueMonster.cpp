// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueMonster.h"
#include "UObjectGlobals.h"
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
}


// Called when the game starts or when spawned
void ABlueMonster::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();
	RightEndLocation = new FVector((StartLocation.X + 200.f), StartLocation.Y, StartLocation.Z);
	LeftEndLocation = new FVector((StartLocation.X - 200.f), StartLocation.Y, StartLocation.Z);
	isGoingRight = true;
}

// Called every frame
void ABlueMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	//FVector x = CharacterSprite->GetPhysicsLinearVelocity();
	//x.Z += 2.f;
	//CharacterSprite->SetPhysicsLinearVelocity(x);

	//CharacterSprite->AddForce(FVector::RightVector * 100000, NAME_None, true);
}

