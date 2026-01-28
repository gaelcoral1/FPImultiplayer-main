// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPIpvmultiGameMode.h"
#include "FPIpvmultiCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "FPIpvmultiGameState.h"
#include "GameFramework/GameStateBase.h"

AFPIpvmultiGameMode::AFPIpvmultiGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

void AFPIpvmultiGameMode::PlayerEnteredWinZone(ACharacter* Player)
{
	if (!HasAuthority() || !Player) return;

	if (PlayersThatArrived.Contains(Player))
		return;

	PlayersThatArrived.Add(Player);

	AFPIpvmultiGameState* GS =
		GetGameState<AFPIpvmultiGameState>();

	if (!GS) return;

	GS->PlayersInWinZone = PlayersThatArrived.Num();

	int32 TotalPlayers = GameState->PlayerArray.Num();

	if (GS->PlayersInWinZone >= TotalPlayers)
	{
		GS->SetGameFinished();
	}
}

void AFPIpvmultiGameMode::RegisterPlayerDetected()
{
	if (!HasAuthority()) return;

	AFPIpvmultiGameState* GS =
		GetGameState<AFPIpvmultiGameState>();

	if (!GS || GS->bGameLost)
		return;

	GS->TimesDetected++;

	if (GS->TimesDetected >= 5)
	{
		GS->SetGameLost();
	}
	
}

void AFPIpvmultiGameMode::ResetTimesDetected()
{
	if (!HasAuthority()) return;

	AFPIpvmultiGameState* GS =
		GetGameState<AFPIpvmultiGameState>();

	if (!GS || GS->bGameLost)
		return;

	GS->TimesDetected = 0;
}

