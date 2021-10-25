// Copyright Epic Games, Inc. All Rights Reserved.

#include "HelicopterGameMode.h"
#include "HelicopterCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHelicopterGameMode::AHelicopterGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
