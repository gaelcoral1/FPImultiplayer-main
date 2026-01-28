// Fill out your copyright notice in the Description page of Project Settings.


#include "FPIpvmultiGameState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

void AFPIpvmultiGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AFPIpvmultiGameState, bGameFinished);
	DOREPLIFETIME(AFPIpvmultiGameState, PlayersInWinZone);
	
	DOREPLIFETIME(AFPIpvmultiGameState, bGameLost);
	DOREPLIFETIME(AFPIpvmultiGameState, TimesDetected);
}

void AFPIpvmultiGameState::SetGameLost()
{
	if (bGameLost)
		return;

	bGameLost = true;

	OnGameLost.Broadcast();
}

void AFPIpvmultiGameState::OnRep_GameFinished()
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Yellow,
		TEXT("JUEGO TERMINADO")
	);
	
	OnGameFinished.Broadcast();
}

void AFPIpvmultiGameState::OnRep_GameLost()
{
	
	OnGameLost.Broadcast();
}


void AFPIpvmultiGameState::SetGameFinished()
{
	if (bGameFinished)
		return;

	bGameFinished = true;

	OnGameFinished.Broadcast();
}

void AFPIpvmultiGameState::OnRep_TimesDetected()
{
	if (!GEngine) return;

	GEngine->AddOnScreenDebugMessage(
		1,                     
		2.0f,                  
		FColor::Orange,
		FString::Printf(
			TEXT("Jugadores detectados: %d / 5"),
			TimesDetected
		)
	);
}


