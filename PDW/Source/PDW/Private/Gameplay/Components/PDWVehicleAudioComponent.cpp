// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Components/PDWVehicleAudioComponent.h"
#include "NebulaVehicle/PDWVehiclePawn.h"

void UPDWVehicleAudioComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeAudioComponents();
	if(APDWVehiclePawn* OwnerPawn = Cast<APDWVehiclePawn>(GetOwner()))
	{
		VehicleOwner = OwnerPawn;
		//VehicleOwner->OnCollision.AddUniqueDynamic(this,);
		//VehicleOwner->OnGroundMaterialChanged.AddUniqueDynamic(this,);
	}

	AudioMgr = UPDWAudioManager::Get(this);
}

void UPDWVehicleAudioComponent::InitializeAudioComponents()
{
	for( auto& [AudioType,AudioStruct] : AudioConfigurations )
	{
		if (AudioStruct.bOneShotSound)
		{
			continue;
		}

		UFMODAudioComponent* AudioComponent = NewObject<UFMODAudioComponent>(this);
		if (AudioComponent)
		{
			AudioComponent->RegisterComponent();
			AudioComponent->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			AudioStruct.AudioComponentInstance = AudioComponent;
			AudioComponent->SetComponentTickEnabled(false);
			AudioComponent->bStopWhenOwnerDestroyed = true;
			AudioComponent->bAutoActivate = false;
			if (AudioStruct.AudioEvent)
			{
				AudioComponent->Event = AudioStruct.AudioEvent;
				//AudioComponent->SetEvent(AudioStruct.AudioEvent);
			}
			else
			{
				UE_LOG(LogTemp,Warning,TEXT("PDWVehicleAudioComponent:Missing event Audio"));
			}
			AudioComponent->Play();
		}

	}
}

void UPDWVehicleAudioComponent::OnVehicleCollision(float SpeedKPH, EPhysicalSurface Surface, FVector HitLocation)
{
	if(bImpactTimerOn)
		return;

	FName ParameterName;
	float ParameterValue;
	float ImpactDelay;
	if (UFMODEvent* CollisionAudioEvent = AudioMgr->GetSurfaceAudioValue(Surface, ParameterName, ParameterValue,ImpactDelay))
	{
		const FTransform HitTransform(FRotator::ZeroRotator, HitLocation, FVector::OneVector);
		UFMODBlueprintStatics::PlayEventAtLocation(this,CollisionAudioEvent,HitTransform,true);
		bImpactTimerOn = true;
		GetWorld()->GetTimerManager().SetTimer(ImpactTimerHandle,this,&UPDWVehicleAudioComponent::ResetImpactTimer,ImpactDelay,false);
	}
}

void UPDWVehicleAudioComponent::OnGroundSurfaceChange(FName NewSurfaceName)
{

}

void UPDWVehicleAudioComponent::ResetImpactTimer()
{
	bImpactTimerOn = false;
}

void UPDWVehicleAudioComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);
	if(!AudioMgr)
		return;

	if(VehicleOwner)
	{
		if(UFMODAudioComponent* EngineAudio = AudioConfigurations.FindRef(EAudioType::Engine).AudioComponentInstance)
		{
			if (!EngineAudio->IsPlaying())
			{
				EngineAudio->Play();
			}
			const float RPM = VehicleOwner->GetRPM();
			EngineAudio->SetParameter(*AudioConfigurations.FindRef(EAudioType::Engine).ParameterName, RPM);
		}
		
		if(UFMODAudioComponent* ReverseAudio = AudioConfigurations.FindRef(EAudioType::Reverse).AudioComponentInstance)
		{
			const float Speed = VehicleOwner->GetSpeedKPH();
			if (Speed <= -1.f)
			{
				if (!ReverseAudio->IsPlaying())
				{
					ReverseAudio->Play();
				}
			}
			else
			{
				ReverseAudio->Stop();
			}
		}

		//Surface audio
		if (UFMODAudioComponent* WheelsAudio = AudioConfigurations.FindRef(EAudioType::Wheels).AudioComponentInstance)
		{
			if (FMath::Abs(VehicleOwner->GetSpeedKPH()) >1)
			{
				bMoving = true;
				if (!WheelsAudio->IsPlaying())
				{
					WheelsAudio->Play();
				}
				const EPhysicalSurface CurrentSurface = VehicleOwner->GetCurrentGroundMaterialName();
				if (CurrentSurface != LastSurface)
				{
					
					LastSurface = CurrentSurface;
					FName ParameterName;
					float ParameterValue;
					float ImpactDelay;
					AudioMgr->GetSurfaceAudioValue(LastSurface, ParameterName, ParameterValue, ImpactDelay);
					WheelsAudio->SetParameter(ParameterName, ParameterValue);
				}
			}
			else
			{
				if (bMoving)
				{
					bMoving=false;
					WheelsAudio->Stop();
				}
			}
		}
		
		if (UFMODAudioComponent* ScreechAudio = AudioConfigurations.FindRef(EAudioType::Screech).AudioComponentInstance)
		{
			if (FMath::Abs(VehicleOwner->GetVelocityVector().Y) > DriftTreshold)
			{
				if (!ScreechAudio->IsPlaying())
				{
					ScreechAudio->Play();
				}
			}
			else
			{
				ScreechAudio->Stop();
			}

		}
	}
}
