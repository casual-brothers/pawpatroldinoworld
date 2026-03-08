// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"


/**
 * 
 */

class WORLDPARTITIONHELPER_API UWPHelperThread : public FRunnable
{


public:

	UWPHelperThread();

	virtual ~UWPHelperThread();

	bool Init() override;
	uint32 Run() override;
	void Stop() override;

private:

	FRunnableThread* Thread;
	bool bRunThread;

	// Add any other member variables or methods you need for your thread here
	// For example, a queue to hold tasks, synchronization primitives, etc.FR
};
