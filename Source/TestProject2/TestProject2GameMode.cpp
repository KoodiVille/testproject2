// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TestProject2GameMode.h"
#include "TestProject2Character.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "PlayerHud.h"
#include "Blueprint/UserWidget.h"

ATestProject2GameMode::ATestProject2GameMode()
{
	// Set default pawn class to our character
	DefaultPawnClass = ATestProject2Character::StaticClass();	

	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBar(TEXT("/Game/UI/Health_UI"));
	HUDWidgetClass = HealthBar.Class;

	// use our custom HUD class
	HUDClass = APlayerHud::StaticClass();

	// add Health Bar UI to viewport
	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void ATestProject2GameMode::BeginPlay()
{
	Super::BeginPlay();

	SetCurrentState(EGamePlayState::EPlaying);

	MyCharacter = Cast<ATestProject2Character>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void ATestProject2GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyCharacter)
	{
		if (FMath::IsNearlyZero(MyCharacter->GetHealth(), 0.001f))
		{
			SetCurrentState(EGamePlayState::EGameOver);
		}
	}
}

EGamePlayState ATestProject2GameMode::GetCurrentState() const
{
	return CurrentState;
}

void ATestProject2GameMode::SetCurrentState(EGamePlayState NewState)
{
	CurrentState = NewState;
	HandleNewState(CurrentState);
}

void ATestProject2GameMode::HandleNewState(EGamePlayState NewState)
{
	if (NewState == EGamePlayState::EGameOver) 
	{
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
}
