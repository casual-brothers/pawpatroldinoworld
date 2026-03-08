// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FLOW/PaleoCenter/PDWFlowNode_FakeDinoNeed.h"
#include "Managers/PDWPaleoCenterSubsystem.h"
#include "Gameplay/Actors/PDWDinoPen.h"

UPDWFlowNode_FakeDinoNeed::UPDWFlowNode_FakeDinoNeed(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{
	#if WITH_EDITOR
	Category = TEXT("PDW|PaleoCenter");
	#endif

}

void UPDWFlowNode_FakeDinoNeed::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	if (UPDWPaleoCenterSubsystem* PaleoCenterSubsystem = GetWorld()->GetSubsystem<UPDWPaleoCenterSubsystem>())
	{
		TArray<UPDWDinoPen*> DinoPens = PaleoCenterSubsystem->GetDinoPens();

		for(UPDWDinoPen* DinoPen : DinoPens)
		{
			if (DinoPen && DinoPen->DinoPenTag.MatchesTagExact(DinoPenTag)) 
			{
				DinoPen->HandleFakeNeed(FakeNeed);
				break;
			}
		}
	}

	TriggerFirstOutput(true);
}
