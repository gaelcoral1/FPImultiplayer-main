// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPIpvmultiGameMode.generated.h"

class AFPIpvmultiCharacter;

UCLASS(minimalapi)
class AFPIpvmultiGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFPIpvmultiGameMode();
	
	void PlayerEnteredWinZone(ACharacter* Player);
	
	void RegisterPlayerDetected();
	
	void ResetTimesDetected();
	
protected:
	UPROPERTY()
	TSet<ACharacter*> PlayersThatArrived;
};



