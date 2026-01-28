// Fill out your copyright notice in the Description page of Project Settings.

#include "FPIpvmulti/Public/AI/AIGuard.h"
#include "FPIpvmulti/FPIpvmultiCharacter.h"
#include "FPIpvmulti/FPIpvmultiGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h" // Necesario para DrawDebugSphere

// Sets default values
AAIGuard::AAIGuard()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	
	GuardState = EIAState::Idle;
}

void AAIGuard::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAIGuard, GuardState);
}

// Called when the game starts or when spawned
void AAIGuard::BeginPlay()
{
	Super::BeginPlay();
	
	OriginalRotator = GetActorRotation();
}

void AAIGuard::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (PawnSensingComp)
	{
		// CRITICAL FIX: Limpiar delegados previos para evitar el "Ensure condition failed" si se reinicializa
		PawnSensingComp->OnSeePawn.RemoveDynamic(this, &ThisClass::OnPawnSeen);
		PawnSensingComp->OnHearNoise.RemoveDynamic(this, &ThisClass::OnNoiseHeard);

		// Vincular delegados de forma segura
		PawnSensingComp->OnSeePawn.AddDynamic(this, &ThisClass::OnPawnSeen);
		PawnSensingComp->OnHearNoise.AddDynamic(this, &ThisClass::OnNoiseHeard);
	}
}

// Called every frame
void AAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AAIGuard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


//ON PAWN SEEN
void AAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr) return;
	
	SeenPawnDetected = SeenPawn;
	if (AFPIpvmultiCharacter* Player = Cast<AFPIpvmultiCharacter>(SeenPawn))
	{
		if (Player->bIsHiddenFromAI)
		{
			return; 
		}
	}
	
	DrawDebugSphere(
		GetWorld(),
		SeenPawn->GetActorLocation(),
		35.0f,
		12.0f,
		FColor::Magenta,
		false,
		6.0f);
	
	SetGuardState(EIAState::Alarted);
	
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Detection))
		return;

	// Cada 1.0 segundos (corregido comentario vs valor real)
	GetWorldTimerManager().SetTimer(
		TimerHandle_Detection,
		this,
		&AAIGuard::AccumulateDetection,
		1.0f,
		true
	);
}

// ACUMULAR DETECCIÓN:
void AAIGuard::AccumulateDetection()
{
	// Si no hay pawn guardado, reset
	if (!SeenPawnDetected)
	{
		ResetDetection();
		return;
	}

	// Verifica si aún tiene línea de visión
	if (!PawnSensingComp->HasLineOfSightTo(SeenPawnDetected))
	{
		ResetDetection();
		return;
	}
	
	AFPIpvmultiCharacter* Player =
		Cast<AFPIpvmultiCharacter>(SeenPawnDetected);

	if (!Player || Player->bIsHiddenFromAI)
	{
		ResetDetection();
		return;
	}

	DetectionPoints++;

	if (HasAuthority())
	{
		if (AFPIpvmultiGameMode* GM =
			GetWorld()->GetAuthGameMode<AFPIpvmultiGameMode>())
		{
			GM->RegisterPlayerDetected();
		}
	}

	// Debug visual
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Orange,
			FString::Printf(
				TEXT("DETECCIÓN ACUMULADA: %d"),
				DetectionPoints
			)
		);
	}
}
// REINICIAR LA DETECCIÓN:
void AAIGuard::ResetDetection()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Detection);
	DetectionPoints = 0;
	SeenPawnDetected = nullptr;

	
	if (HasAuthority())
	{
		if (AFPIpvmultiGameMode* GM =
			GetWorld()->GetAuthGameMode<AFPIpvmultiGameMode>())
		{
			GM->ResetTimesDetected();
		}
	}
	
	if (GuardState != EIAState::Idle)
	{
		SetGuardState(EIAState::Idle);
	}
}

//ON NOISE HEARD
void AAIGuard::OnNoiseHeard(APawn* HearInstigator, const FVector& Location, float Volume)
{
	if (HearInstigator)
	{
		if (AFPIpvmultiCharacter* Player = Cast<AFPIpvmultiCharacter>(HearInstigator))
		{
			if (Player->bIsHiddenFromAI)
			{
				return; 
			}
		}
	}
	//if (HearInstigator == nullptr) return;
	DrawDebugSphere(
		GetWorld(),
		Location,
		30.0f,
		12.0f,
		FColor::Blue,
		false,
		6.0f);
										
	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();
	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &ThisClass::ResetOrientation, 3.0f);
	
	SetGuardState(EIAState::Suspicious);
}

void AAIGuard::ResetOrientation()
{
	if (GuardState == EIAState::Alarted) return;
	
	SetActorRotation(OriginalRotator);
	SetGuardState(EIAState::Idle);
}

void AAIGuard::SetGuardState(EIAState NewState)
{
	if (GuardState == NewState) return;
	GuardState = NewState;
	OnStateChanged(GuardState);
	
}


void AAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}


void AAIGuard::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.RemoveAll(this);
		PawnSensingComp->OnHearNoise.RemoveAll(this);
	}

	GetWorldTimerManager().ClearAllTimersForObject(this);
}
