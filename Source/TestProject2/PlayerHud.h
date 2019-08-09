#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHud.generated.h"


UCLASS()
class APlayerHud : public AHUD
{
	GENERATED_BODY()
public:
	APlayerHud();

	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnyWhere, Category = "Health")
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnyWhere, Category = "Health")
	class UUserWidget* CurrentWidget;
};
