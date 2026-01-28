// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AWinTrigger.generated.h"
class UBoxComponent;
UCLASS()
class FPIPVMULTI_API AAWinTrigger : public AActor
{
	GENERATED_BODY()

public:
	AAWinTrigger();

protected:
	virtual void BeginPlay() override;

	
protected:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerBox;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
