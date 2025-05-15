// Copyright Epic Games, Inc. All Rights Reserved.

#include "gameGameMode.h"
#include "gameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AgameGameMode::AgameGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
