// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PDWLocalPlayer.h"
#include "Data/PDWPersistentUser.h"

UPDWLocalPlayer::UPDWLocalPlayer(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PersistentUserClass = UPDWPersistentUser::StaticClass();
}

FString UPDWLocalPlayer::GetDefaultSlotName() const
{
#if defined(IS_3DC_EOS) 
	return FString("PDWGameProgressionEos");
#else
	return FString("PDWGameProgression");
#endif
}

void UPDWLocalPlayer::PlayerAdded(class UGameViewportClient* InViewportClient, int32 InControllerID)
{
	Super::PlayerAdded(InViewportClient, InControllerID);
}

void UPDWLocalPlayer::OnPersistentUserLoaded(const bool& bNewPersistentUser)
{
	Super::OnPersistentUserLoaded(bNewPersistentUser);
	PDWPersistentUser = Cast<UPDWPersistentUser>(GetPersistentUser());
}
