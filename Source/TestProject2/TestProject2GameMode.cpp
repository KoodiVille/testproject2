// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TestProject2GameMode.h"
#include "TestProject2Character.h"

ATestProject2GameMode::ATestProject2GameMode()
{
	// Set default pawn class to our character
	DefaultPawnClass = ATestProject2Character::StaticClass();	
}
