// Fill out your copyright notice in the Description page of Project Settings.


#include "MedKit.h"
#include "TestProject2Character.h"

// Sets default values
AMedKit::AMedKit()
{
 	OnActorBeginOverlap.AddDynamic(this, &AMedKit::OnOverlap);
}

void AMedKit::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor)
{
	if (OtherActor != nullptr && OtherActor != this) 
	{
		class ATestProject2Character* MyCharacter = Cast<ATestProject2Character>(OtherActor);

		if (MyCharacter && MyCharacter->GetHealth() < 1.0f)
		{
			MyCharacter->UpdateHealth(100.0f);
			Destroy();
		}
	}
}
