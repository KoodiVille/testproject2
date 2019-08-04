// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Paper2D\Classes\PaperSpriteComponent.h>
#include "BlueMonster.generated.h"

UCLASS()
class TESTPROJECT2_API ABlueMonster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlueMonster();
	ABlueMonster(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	UPROPERTY(EditAnywhere)
	UPaperSpriteComponent* CharacterSprite;
	FVector StartLocation;
	FVector* RightEndLocation;
	FVector* LeftEndLocation;
	bool isGoingRight;
};
