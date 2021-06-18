// Copyright Epic Games, Inc. All Rights Reserved.

#include "un1verse_ntwrk_dmoGameMode.h"
#include "un1verse_ntwrk_dmoHUD.h"
#include "un1verse_ntwrk_dmoCharacter.h"
#include "UObject/ConstructorHelpers.h"

Aun1verse_ntwrk_dmoGameMode::Aun1verse_ntwrk_dmoGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = Aun1verse_ntwrk_dmoHUD::StaticClass();
}
