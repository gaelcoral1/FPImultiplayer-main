// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIGuard.generated.h"

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EIAState : uint8
{
	Idle, 
	Suspicious,
	Alarted
};

UCLASS()
class FPIPVMULTI_API AAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAIGuard();
	
	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// SENSINGS
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	void ResetDetection();
	void AccumulateDetection();
	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void OnNoiseHeard(APawn* HearInstigator, const FVector& Location, float Volume);
	
	FRotator OriginalRotator;
	
	UFUNCTION()
	void ResetOrientation();
	
	FTimerHandle TimerHandle_ResetOrientation;
	
	// GUARD STATE:
	
	UPROPERTY(ReplicatedUsing=OnRep_GuardState)
	EIAState GuardState;
	
	UFUNCTION()
	void OnRep_GuardState();
	
	void SetGuardState(EIAState NewState);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "IA Guard")
	void OnStateChanged(EIAState NewState);
	
protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	
	FTimerHandle TimerHandle_Detection;

	UPROPERTY()
	APawn* SeenPawnDetected = nullptr;

	int32 DetectionPoints = 0;
	
};
