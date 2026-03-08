// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "PDWAudioManager.h"
#include "PDWDialogueSubSystem.generated.h"

#pragma region Dialogues Structures

class APlayerController;

UENUM(BlueprintType)
enum class EEmotionType : uint8
{
	Idle = 0,
	Happy = 1,
	Worried = 2,
	Surprised = 3,
	Think = 4,
	Serious = 5,
	Sad = 6,
	Angry = 7,
	Evil = 8
};

UENUM(BlueprintType)
enum class EConversationType : uint8
{
	None = 0 UMETA(Hidden),
	BlockingDialogue = 1,
	HUDComment = 2,
	Cutscene = 3,
};

USTRUCT(BlueprintType)
struct PDW_API FAnswer
{
	GENERATED_USTRUCT_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayAnswer {};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionAnswerName {};
};

USTRUCT(BlueprintType)
struct PDW_API FDIalogueCameraConfig
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraBlendTime = 0.5f;
};

USTRUCT(BlueprintType)
struct PDW_API FDIalogueAnimationConfig
{
	GENERATED_USTRUCT_BODY()

public: 
	
	UPROPERTY(EditAnywhere, Category = "Animation")
	EEmotionType AnimationMood = EEmotionType::Happy;
};

USTRUCT(BlueprintType)
struct PDW_API FDialogueLine
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DESIGN", meta=(MultiLine=true))
	FText DialogueLine = FText();

	UPROPERTY(EditAnywhere, Category = "Configuration", meta=(GetOptions = "PDWUIFunctionLibrary.GetAllDTSpeakersRows"))
	FName SpeakerID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DESIGN")
	FVoiceOverDialogueStruct VoiceOverConfig = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DESIGN")
	FText OveridedDisplaySpeakerName = FText();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DESIGN")
	FGameplayTagContainer FramedCharacter {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DESIGN | CameraConfig")
	FDIalogueCameraConfig DialogueCameraConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DESIGN | AnimationConfig")
	FDIalogueAnimationConfig DialogueAnimationConfig;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DESIGN")
	//	bool bIsDialogueAnswer = false;
	//	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsDialogueAnswer == true", EditConditionHides, DisplayName="Answers"), Category = "DESIGN")
	//	TArray<FAnswer> Answers = {};

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DESIGN")
	//	TMap<FName, EIdleMode> SpeakersAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DESIGN")
		bool bIsLastSentence = false;
};

USTRUCT(BlueprintType)
struct PDW_API FConversation
{
	GENERATED_USTRUCT_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EConversationType ConversationType = EConversationType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="ConversationType==EConversationType::HUDComment", EditConditionHides))
	bool bLoopConversation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="ConversationType==EConversationType::HUDComment && bLoopConversation==true", EditConditionHides))
	float LoopTime = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDialogueLine> DialogueLines = TArray<FDialogueLine>();
};

USTRUCT(BlueprintType)
struct PDW_API FCharacterInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText DisplayCharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UTexture2D> CharacterImage = {};
};

USTRUCT(BlueprintType)
struct PDW_API FCharacterInfoRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCharacterInfo CharacterInfo;
};

#pragma endregion

/**
 * 
 */
UCLASS()
class PDW_API UPDWDialogueSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:

	DECLARE_MULTICAST_DELEGATE(FOnConversationCompleted);
	
	static UPDWDialogueSubSystem* Get(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext"))
	static void TriggerSubtitle(UObject* WorldContext, FDialogueLine inDialogueLine);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext"))
	static void SkipSubtitle(UObject* WorldContext);

	UFUNCTION(BlueprintCallable)
	static void TriggerConversation(UObject* WorldContext, const FConversation&  inConversationConfig);

	UFUNCTION(BlueprintCallable)
	static void TriggerDialogueLineAnimations(UObject* WorldContext, const FDialogueLine& inDialogConfig);

	UFUNCTION(BlueprintCallable)
	static void CloseConversation(UObject* WorldContext);
	
	static FOnConversationCompleted OnConversationCompleted;

	UPROPERTY(BlueprintReadOnly)
	FConversation CurrentConversation;

	UPROPERTY()
	APlayerController* PlayerDoingDialogue;

protected:
	UFUNCTION(BlueprintCallable)
	void TriggerCamera(AActor* inSpeakerActor,const FDialogueLine& inDialogConfig);

	UFUNCTION(BlueprintCallable)
	void TriggerAnimation(AActor* inSpeakerActor,const FDialogueLine& inDialogConfig);

	UFUNCTION(BlueprintCallable)
	void ResetAnimation(AActor* inSpeakerActor);

	UFUNCTION(BlueprintCallable)
	void ShowConversation(const FConversation&  inConversationConfig);
	
private:

	EEmotionType LastAnimationMood;
	FGameplayTagContainer LastSpeakerID;
	UPROPERTY()
	AActor* LastSpeakerActor = nullptr;
};
