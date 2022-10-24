// Copyright Epic Games, Inc. All Rights Reserved.

#include "FootPlacementGameMode.h"
#include "FootPlacementCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFootPlacementGameMode::AFootPlacementGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
