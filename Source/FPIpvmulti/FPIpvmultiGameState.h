// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPIpvmultiGameState.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnGameFinished);
DECLARE_MULTICAST_DELEGATE(FOnGameLost);

UCLASS()
class FPIPVMULTI_API AFPIpvmultiGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(ReplicatedUsing = OnRep_GameFinished)
	bool bGameFinished = false;
	
	UPROPERTY(ReplicatedUsing = OnRep_GameLost)
	bool bGameLost = false;

	UPROPERTY(Replicated)
	int32 PlayersInWinZone = 0;
		
	UPROPERTY(ReplicatedUsing = OnRep_TimesDetected)
	int32 TimesDetected = 0;
	
	FOnGameFinished OnGameFinished;
	
	FOnGameLost OnGameLost;

	UFUNCTION()
	void SetGameLost();
	
	UFUNCTION()
	void OnRep_GameFinished();
	
	UFUNCTION()
	void OnRep_GameLost();

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;
	
	UFUNCTION()
	void SetGameFinished();
	
	UFUNCTION()
	void OnRep_TimesDetected();

};
