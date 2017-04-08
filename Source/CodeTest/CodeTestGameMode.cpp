// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "CodeTest.h"
#include "CodeTestGameMode.h"
#include "CodeTestHUD.h"
#include "CodeTestCharacter.h"

ACodeTestGameMode::ACodeTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ACodeTestHUD::StaticClass();
}
