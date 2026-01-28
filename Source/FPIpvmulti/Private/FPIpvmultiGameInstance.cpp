#include "FPIpvmultiGameInstance.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Online/OnlineSessionNames.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"

void UFPIpvmultiGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}

	DestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete);
	CreateSessionCompleteDelegate  = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete);
	FindSessionsCompleteDelegate   = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete);
	JoinSessionCompleteDelegate    = FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete);
}

void UFPIpvmultiGameInstance::HostSession(int32 PublicConnections)
{
	if (!SessionInterface.IsValid())
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SessionInterface invalid"));
		return;
	}

	PendingPublicConnections = FMath::Clamp(PublicConnections, 1, 64);

	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		DestroySessionCompleteHandle =
			SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

		SessionInterface->DestroySession(NAME_GameSession);
		return;
	}

	CreateSession_Internal();
}

void UFPIpvmultiGameInstance::CreateSession_Internal()
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	CreateSessionCompleteHandle =
		SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	TSharedPtr<FOnlineSessionSettings> Settings = MakeShared<FOnlineSessionSettings>();

	const FName SubsystemName = IOnlineSubsystem::Get()->GetSubsystemName();
	Settings->bIsLANMatch = (SubsystemName == "NULL");
	Settings->NumPublicConnections = PendingPublicConnections;

	Settings->bAllowJoinInProgress = true;
	Settings->bShouldAdvertise = true;

	Settings->bUsesPresence = true;
	Settings->bAllowJoinViaPresence = true;
	Settings->bUseLobbiesIfAvailable = true;

	Settings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings->Set(FName("ProjectTag"), ProjectTag, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	const ULocalPlayer* LP = GetFirstGamePlayer();
	if (!LP)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No LocalPlayer"));
		return;
	}

	SessionInterface->CreateSession(*LP->GetPreferredUniqueNetId(), NAME_GameSession, *Settings);
}

void UFPIpvmultiGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteHandle);
	}

	CreateSession_Internal();
}

void UFPIpvmultiGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
	}

	if (!bWasSuccessful)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("CreateSession failed"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FString TravelURL = GameplayMapPath + TEXT("?listen");
	World->ServerTravel(TravelURL);
}

void UFPIpvmultiGameInstance::FindAndJoinTestSession()
{
	if (!SessionInterface.IsValid())
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SessionInterface invalid"));
		return;
	}

	SessionSearch = MakeShared<FOnlineSessionSearch>();
	SessionSearch->MaxSearchResults = 200;
	SessionSearch->bIsLanQuery = false;

	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(FName("ProjectTag"), ProjectTag, EOnlineComparisonOp::Equals);

	FindSessionsCompleteHandle =
		SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	const ULocalPlayer* LP = GetFirstGamePlayer();
	if (!LP)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No LocalPlayer"));
		return;
	}

	SessionInterface->FindSessions(*LP->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void UFPIpvmultiGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
	}

	if (!bWasSuccessful || !SessionSearch.IsValid())
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("FindSessions failed"));
		return;
	}

	for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
	{
		FString FoundMatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), FoundMatchType);

		FString FoundProjectTag;
		Result.Session.SessionSettings.Get(FName("ProjectTag"), FoundProjectTag);

		if (FoundProjectTag == ProjectTag && FoundMatchType == MatchType)
		{
			JoinSessionCompleteHandle =
				SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

			const ULocalPlayer* LP = GetFirstGamePlayer();
			if (!LP)
			{
				return;
			}

			FOnlineSessionSearchResult ResultToJoin = Result;
			if (ResultToJoin.Session.SessionSettings.bUsesPresence)
			{
				ResultToJoin.Session.SessionSettings.bUseLobbiesIfAvailable = true;
			}

			SessionInterface->JoinSession(*LP->GetPreferredUniqueNetId(), NAME_GameSession, ResultToJoin);
			return;
		}
	}

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("No matchin session found"));
}

void UFPIpvmultiGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("JoinSession failed"));
		return;
	}

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address) || Address.IsEmpty())
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Red, TEXT("Could not resolve connect string"));
		return;
	}

	APlayerController* PC = GetFirstLocalPlayerController();
	if (PC)
	{
		PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Yellow,
			*FString::Printf(TEXT("Join Result: %d"), (int32)Result));
	}
}