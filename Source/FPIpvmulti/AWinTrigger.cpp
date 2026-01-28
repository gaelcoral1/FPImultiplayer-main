// Fill out your copyright notice in the Description page of Project Settings.


#include "AWinTrigger.h"
#include "Components/BoxComponent.h"
#include "FPIpvmultiCharacter.h"
#include "FPIpvmultiGameMode.h"

// Sets default values
AAWinTrigger::AAWinTrigger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

// Called when the game starts or when spawned
void AAWinTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAWinTrigger::OnOverlapBegin);
}

void AAWinTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	ACharacter* Char = Cast<ACharacter>(OtherActor);
	if (!Char) return;

	AFPIpvmultiGameMode* GM =
		GetWorld()->GetAuthGameMode<AFPIpvmultiGameMode>();

	if (GM)
	{
		GM->PlayerEnteredWinZone(Char);
	}
}

