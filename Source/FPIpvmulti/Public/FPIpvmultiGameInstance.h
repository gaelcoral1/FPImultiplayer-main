#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "FPIpvmultiGameInstance.generated.h"

UCLASS()
class FPIPVMULTI_API UFPIpvmultiGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category="Online|Session")
	void HostSession(int32 PublicConnections = 4);
	
	UFUNCTION(BlueprintCallable, Category="Online|Session")
	void FindAndJoinTestSession();

protected:
	UPROPERTY(EditDefaultsOnly, Category="Online|Travel")
	FString GameplayMapPath = TEXT("/Game/FirstPerson/Maps/FirstPersonMap");
	
	UPROPERTY(EditDefaultsOnly, Category="Online|Session")
	FString ProjectTag = TEXT("FPIpvmulti");

	UPROPERTY(EditDefaultsOnly, Category="Online|Session")
	FString MatchType = TEXT("Test");

private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	int32 PendingPublicConnections = 4;

	// Delegates
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteHandle;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteHandle;

	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteHandle;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteHandle;


	void CreateSession_Internal();
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
};
