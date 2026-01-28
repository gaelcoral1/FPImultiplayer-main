// Fill out your copyright notice in the Description page of Project Settings.
#include "FPIpvmultiPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "FPIpvmultiGameState.h"

void AFPIpvmultiPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController())
		return;

	AFPIpvmultiGameState* GS =
		GetWorld()->GetGameState<AFPIpvmultiGameState>();

	if (GS)
	{
		GS->OnGameFinished.AddUObject(
			this,
			&AFPIpvmultiPlayerController::HandleGameFinished
		);
		GS->OnGameLost.AddUObject(
			this,
			&AFPIpvmultiPlayerController::HandleLostGame);
	}
}

void AFPIpvmultiPlayerController::HandleGameFinished()
{
	if (!IsLocalController()) return;

	UUserWidget* WinWidget =
		CreateWidget(this, WinWidgetClass);

	if (WinWidget)
	{
		WinWidget->AddToViewport();
	}
}

void AFPIpvmultiPlayerController::HandleLostGame()
{
	if (!IsLocalController()) return;

	UUserWidget* LoseWidget =
		CreateWidget(this, LoseWidgetClass);

	if (LoseWidget)
	{
		LoseWidget->AddToViewport();
	}
	
	PauseAndEndGame(2.f);
}

void AFPIpvmultiPlayerController::PauseAndEndGame(float Delay)
{
	if (!IsLocalController())
		return;

	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	bShowMouseCursor = true;

	GetWorldTimerManager().SetTimer(
		TimerHandle_EndGame,
		this,
		&AFPIpvmultiPlayerController::EndGame,
		Delay,
		false
	);
}

void AFPIpvmultiPlayerController::EndGame()
{
	if (!IsLocalController())
		return;
	ConsoleCommand(TEXT("quit"));
}
