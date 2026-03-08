#include "Core/NebulaFlowPlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/LocalPlayer.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineStats.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "NebulaFlow.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Core/NebulaFlowGameInstance.h"
#include "Managers/NebulaFlowAchievementManager.h"
#include "FunctionLibraries/NebulaFlowSingletonFunctionLibrary.h"


const FName ANebulaFlowPlayerController::ACTION_BACK = FName("Back");
const FName ANebulaFlowPlayerController::ACTION_BACK_JP = FName("Confirm");



ANebulaFlowPlayerController::ANebulaFlowPlayerController(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	
}

void ANebulaFlowPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this, nullptr, false, false);
	FInputModeGameAndUI inputMode;
	inputMode.SetHideCursorDuringCapture(false);
	SetInputMode(inputMode);
}

void ANebulaFlowPlayerController::UnlockAchievement(const FName& Id)
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer)
	{
		IOnlineSubsystem* OnlineSub = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem();
		if (OnlineSub)
		{
			IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
			if (Identity.IsValid())
			{
				TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->GetControllerId());
				//FUniqueNetIdRepl UserId = LocalPlayer->GetCachedUniqueNetId(); //old UserID from PAW

				if (UserId.IsValid())
				{
					IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();
					if (Achievements.IsValid())
					{
						AchievementQueue.AddUnique(Id);
					}
					else
					{
						UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("Write Object is not valid"));
					}
				}
				else
				{
					UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid user id for this controller."));
				}
			}
			else
			{
				UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid identity interface."));
			}
		}
		else
		{
			UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No default online subsystem."));
		}
	}
	else
	{
		UE_LOG(LogOnline, Warning, TEXT("No local player, cannot update achievements."));
	}
}


bool ANebulaFlowPlayerController::InnerUnlockAchievement(const FName& Id)
{
	bool result = false;
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer)
	{
		IOnlineSubsystem* OnlineSub = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem();
		if (OnlineSub)
		{
			IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
			if (Identity.IsValid())
			{
				TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->GetControllerId());
				//FUniqueNetIdRepl UserId = LocalPlayer->GetCachedUniqueNetId(); //old UserID from PAW

				if (UserId.IsValid())
				{

					IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();
					if (Achievements.IsValid() && (!WriteObject.IsValid() || WriteObject->WriteState != EOnlineAsyncTaskState::InProgress))
					{
						WriteObject = MakeShareable(new FOnlineAchievementsWrite());
						WriteObject->SetFloatStat(Id, 100.0f);

						FOnlineAchievementsWriteRef WriteObjectRef = WriteObject.ToSharedRef();
						Achievements->WriteAchievements(*UserId, WriteObjectRef);
						result = true;
					}
					else
					{
						UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("Write Object is not valid"));
					}
				}
				else
				{
					UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid user id for this controller."));
				}
			}
			else
			{
				UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid identity interface."));
			}
		}
		else
		{
			UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No default online subsystem."));
		}
	}
	else
	{
		UE_LOG(LogOnline, Warning, TEXT("No local player, cannot update achievements."));
	}
	return result;
}


void ANebulaFlowPlayerController::OnBackPressed()
{
}

void ANebulaFlowPlayerController::OnBackAction()
{
}

void ANebulaFlowPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (AchievementQueue.Num() > 0)
	{
		AchievementTimer += DeltaTime;
		if (AchievementTimer >= AchievementMaxTimer)
		{
			AchievementTimer = 0.f;
			const FName CurrentCandidateAchievement = AchievementQueue[0];
			bool requestSent = InnerUnlockAchievement(CurrentCandidateAchievement);
			if (requestSent)
			{
				AchievementQueue.Remove(CurrentCandidateAchievement);
			}
		}
	}

	if (AchievementReadTimer > 0)
	{
		AchievementReadTimer -= DeltaTime;
	}
	else
	{
		AchievementReadTimer = 0.0f;
	}
}

void ANebulaFlowPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UNebulaFlowGameInstance* GameInstance = Cast<UNebulaFlowGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance)
	{
		if (!GameInstance->GetIsPS_JP())
		{
			FInputActionBinding& BackPressBind = InputComponent->BindAction(ACTION_BACK, IE_Pressed, this, &ANebulaFlowPlayerController::OnBackPressed);
			BackPressBind.bConsumeInput = false;
			BackPressBind.bExecuteWhenPaused = true;
			FInputActionBinding& BackReleaseBind = InputComponent->BindAction(ACTION_BACK, IE_Released, this, &ANebulaFlowPlayerController::OnBackAction);
			BackReleaseBind.bConsumeInput = false;
			BackReleaseBind.bExecuteWhenPaused = true;
		}
		else
		{
			FInputActionBinding& BackPressBind = InputComponent->BindAction(ACTION_BACK_JP, IE_Pressed, this, &ANebulaFlowPlayerController::OnBackPressed);
			BackPressBind.bConsumeInput = false;
			BackPressBind.bExecuteWhenPaused = true;
			FInputActionBinding& BackReleaseBind = InputComponent->BindAction(ACTION_BACK_JP, IE_Released, this, &ANebulaFlowPlayerController::OnBackAction);
			BackReleaseBind.bConsumeInput = false;
			BackReleaseBind.bExecuteWhenPaused = true;
		}
	}
}

void ANebulaFlowPlayerController::QueryAchievements(bool bWasOffline)
{
#if !PLATFORM_SWITCH
	if (AchievementReadTimer > 0.0f)
	{
		UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("Too many QueryAchievements call in the same second"));
		return;
	}
	AchievementReadTimer = AchievementReadMaxTimer;
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer && LocalPlayer->GetControllerId() != -1)
	{
		IOnlineSubsystem* OnlineSub = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem();
		if (OnlineSub)
		{
			IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
			if (Identity.IsValid())
			{
				TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->GetControllerId());

				if (UserId.IsValid())
				{
					IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();

					if (Achievements.IsValid())
					{
						if (bWasOffline)
						{
							Achievements->QueryAchievements(*UserId.Get(), FOnQueryAchievementsCompleteDelegate::CreateUObject(this, &ANebulaFlowPlayerController::OnQueryAchievementsCompleteOffline));
						}
						else
						{
							Achievements->QueryAchievements(*UserId.Get(), FOnQueryAchievementsCompleteDelegate::CreateUObject(this, &ANebulaFlowPlayerController::OnQueryAchievementsComplete));
						}
					}
				}
				else
				{
					UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid user id for this controller."));
				}
			}
			else
			{
				UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid identity interface."));
			}
		}
		else
		{
			UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No default online subsystem."));
		}
	}
	else
	{
		UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No local player, cannot read achievements."));
	}
#endif
}

void ANebulaFlowPlayerController::OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful)
{
	UE_LOG(LogNebulaFlowOnLine, Display, TEXT("AFlowPlayerController::OnQueryAchievementsComplete(bWasSuccessful = %s)"), bWasSuccessful ? TEXT("TRUE") : TEXT("FALSE"));

	UNebulaFlowAchievementManager* AchievementManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAchievementManager(GetWorld());

	if (AchievementManager && AchievementManager->bSyncronizeOnlineAchievement)
	{
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
		if (LocalPlayer && LocalPlayer->GetControllerId() != -1)
		{
			IOnlineSubsystem* OnlineSub = UNebulaFlowCoreFunctionLibrary::GetOnlineSubsytem();
			if (OnlineSub)
			{
				IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
				if (Identity.IsValid())
				{
					TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->GetControllerId());

					if (UserId.IsValid())
					{
						IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();

						if (Achievements.IsValid())
						{
							TArray<FOnlineAchievement> ServerAchievements;
							Achievements->GetCachedAchievements(*UserId.Get(), ServerAchievements);
							for (FOnlineAchievement CurrentAchievement : ServerAchievements)
							{
								if (CurrentAchievement.Progress >= 100)
								{
									AchievementManager->UnlockedServerAchievements.AddUnique(FName(CurrentAchievement.Id));
								}
							}
						}
					}
					else
					{
						UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid user id for this controller."));
					}
				}
				else
				{
					UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No valid identity interface."));
				}
			}
			else
			{
				UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No default online subsystem."));
			}
		}
		else
		{
			UE_LOG(LogNebulaFlowOnLine, Warning, TEXT("No local player, cannot read achievements."));
		}
	}
}


void ANebulaFlowPlayerController::OnQueryAchievementsCompleteOffline(const FUniqueNetId& PlayerId, const bool bWasSuccessful)
{
	OnQueryAchievementsComplete(PlayerId, bWasSuccessful);

#if (PLATFORM_XBOXONE || PLATFORM_XSX || PLATFORM_WINGDK)
	UNebulaFlowAchievementManager* AchievementManager = UNebulaFlowSingletonFunctionLibrary::GetNebulaFlowAchievementManager(GetWorld());

	if (AchievementManager)
	{
		AchievementManager->UpdateOfflineAchievements();
	}
#endif
}