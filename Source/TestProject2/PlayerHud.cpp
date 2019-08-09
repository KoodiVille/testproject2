#include "PlayerHud.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

APlayerHud::APlayerHud()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarObj(
		TEXT("/Game/UI/Health_UI")
	);

	HUDWidgetClass = HealthBarObj.Class;
}

void APlayerHud::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass != nullptr) 
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}
}
