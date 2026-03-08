
#pragma once


#include "UObject/Object.h"
#include "Interfaces/OnlineGameActivityInterface.h"
#include "NebulaFlowIntentManager.generated.h"

class UNebulaFlowBasePage;
class UNebulaFlowLocalPlayer;
class ANebulaFlowPlayerController;
class UNebulaFlowDialog;
class UGameInstance;


DECLARE_LOG_CATEGORY_EXTERN(LogPlayerIntents, Log, All);

class UDataTable;

UENUM(BlueprintType)
enum class EIntentType : uint8		
{
	EOfflineActivity,
	EOnlineActivity

};

USTRUCT(BlueprintType)
struct NEBULAFLOW_API FIntentData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName IntentId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ActivityId {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EIntentType IntentType = EIntentType::EOfflineActivity;

	TFunction<void()> IntentCallBack;

};

UCLASS(BlueprintType, Blueprintable)
class NEBULAFLOW_API UNebulaFlowIntentManager : public UObject
{

	friend class UNebulaFlowGameInstance;
	
	GENERATED_UCLASS_BODY()


public:

	UFUNCTION(BlueprintCallable, Category = "Intents", meta = (WorldContext = "WorldContextObject"))
		static UNebulaFlowIntentManager* GetFlowIntentManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Intents", meta = (WorldContext = "WorldContextObject"))
		TArray<FName> GetCurrentManagedIntents() const;


private:

	bool bIsManagingIntent = false;

	TArray<FName> CurrentManagedIntents;

	FOnGameActivityActivationRequestedDelegate ActivityActivationIntentHandler;

public:

	UFUNCTION(BlueprintCallable, Category = "Intents")
		bool GetIsManagingIntent() { return bIsManagingIntent; }

	UFUNCTION(BlueprintCallable, Category = "Intents")
		void SetCanManagerIntent(bool InCanManage) { bCanManageintent = InCanManage; }

	UFUNCTION(BlueprintCallable, Category = "Intents")
		bool GetCanManageintent() { return  bCanManageintent; }

	/*Use this function to Define a new intent that can be handled by the game*/
	void DefineNewIntent(FName InIntentId, EIntentType InType = EIntentType::EOfflineActivity, FString InActivityId = FString(""), TFunction<void(void)> InCallBack = [] {})
	{
		if (!ManagebleIntents.Contains(InIntentId))
		{
			FIntentData NewIntent;
			NewIntent.ActivityId = InActivityId;
			NewIntent.IntentId = InIntentId;
			NewIntent.IntentType = InType;
			NewIntent.IntentCallBack = InCallBack;
			ManagebleIntents.Add(InIntentId, NewIntent);
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Intents")
		void EndManagingIntent(FName InIntentId)
	{
		if (CurrentManagedIntents.Contains(InIntentId))
		{
			CurrentManagedIntents.Remove(InIntentId);
		}
		if (CurrentManagedIntents.Num() == 0)
		{
			bIsManagingIntent = false;
		}
	}

protected:

	virtual void Init();

	virtual void InitOfflineActivityIntents();

	virtual void InitOnlineActivityIntents();

	virtual void OnActivityIntent_Handler(const FUniqueNetId& LocalUserId, const FString& ActivityId, const FOnlineSessionSearchResult* SessionInfo);

	TMap<FName, FIntentData> ManagebleIntents;

	UPROPERTY(EditDefaultsOnly, Category = "Intents")
		UDataTable* OfflineActivityTable;

	void StartManagingIntent(FName InIntentId)
	{
		CurrentManagedIntents.Add(InIntentId);
		bIsManagingIntent = true;
	}

private:

	virtual void OnActivityIntentReceived(const FUniqueNetId& LocalUserId, const FString& ActivityId, const FOnlineSessionSearchResult* SessionInfo);

	bool bHasBeenInitialized = false;

	bool bCanManageintent = true;

};