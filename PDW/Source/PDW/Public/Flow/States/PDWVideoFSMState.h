// Copyright (c) 3DClouds, S.r.l. All rights reserved. Unauthorized copying od this file, via any medium is strictly prohibited

#pragma once

#include "FSM/States/NebulaFlowBaseUIFSMState.h"
#include "FMODBlueprintStatics.h"
#include "PDWVideoFSMState.generated.h"

class UFMODEvent;
class UBinkMediaPlayer;
class UBinkMediaTexture;
class UFileMediaSource;
class UMediaPlayer;

USTRUCT(BlueprintType)
struct PDW_API FPDWVideoAndAudio
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Video")
	TSoftObjectPtr<UFileMediaSource> SwitchVideoToPlay = nullptr;

	UPROPERTY(EditAnywhere, Category = "Video")
	TSoftObjectPtr<UBinkMediaPlayer> BinkVideo{ nullptr };
			
	UPROPERTY(EditAnywhere, Category = "Video")
	UFMODEvent* VideoSound{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Video")
	bool bSkippable{ true };
};

UCLASS(MinimalAPI, Abstract)
class UPDWVideoFSMState : public UNebulaFlowBaseUIFSMState
{
	GENERATED_BODY()

	virtual void OnFSMStateEnter_Implementation(const FString& InOption = FString(""));

	void OnFSMStateAction_Implementation(const FString& Action, const FString& Parameter, APlayerController* ControllerSender) override;
	void StartNextVideo();

	UFUNCTION()
	void OnVideoEnded();
		
protected:

	UPROPERTY(EditAnywhere, Category = "Video")
	UMediaPlayer* MediaPlayer{};

	UPROPERTY(EditAnywhere, Category = "Video")
	TSoftObjectPtr<UBinkMediaTexture> BinkMediaTexture{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Video")
	TArray<FPDWVideoAndAudio> Videos{};
				
	int32 CurrentVideoIndex{-1};

	UPROPERTY()
	FFMODEventInstance Instance;

    UPROPERTY(EditAnywhere, Category = "Editor", meta = (OnlyEditInline, OnlyEditor))
	bool bSkipVideos{ true };
};
