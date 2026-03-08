// Fill out your copyright notice in the Description page of Project Settings.


#include "WPHelperThread.h"

UWPHelperThread::UWPHelperThread()
{
	Thread = FRunnableThread::Create(this, TEXT("UWPHelperThread"));
}

UWPHelperThread::~UWPHelperThread()
{
	if (Thread)
	{
		Thread->Kill();
		delete Thread;
	}
}

bool UWPHelperThread::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("My custom thread has been initialized"))
	bRunThread = true;
	// Return false if you want to abort the thread
	return true;
}

uint32 UWPHelperThread::Run()
{
	while (bRunThread)
	{
		UE_LOG(LogTemp, Warning, TEXT("My custom thread is running!"))
		FPlatformProcess::Sleep(1.0f);
	}

	return 0;
}

void UWPHelperThread::Stop()
{
	bRunThread = false;
}
