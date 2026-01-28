

#include "HideTrigger.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "FPIpvmultiCharacter.h"

AHideTrigger::AHideTrigger()
{
	PrimaryActorTick.bCanEverTick = true;
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>("TriggerBox");
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionProfileName("Trigger");
}

void AHideTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
}


void AHideTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	if (AFPIpvmultiCharacter* Player = Cast<AFPIpvmultiCharacter>(OtherActor))
	{
		Player->bIsHiddenFromAI = true;
	}
}

void AHideTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;

	if (AFPIpvmultiCharacter* Player = Cast<AFPIpvmultiCharacter>(OtherActor))
	{
		Player->bIsHiddenFromAI = false;
	}
}

