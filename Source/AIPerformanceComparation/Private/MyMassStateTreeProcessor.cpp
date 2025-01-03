// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMassStateTreeProcessor.h"
#include "MassSignalSubsystem.h"

void UMyMassStateTreeProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);
	
	UMassSignalSubsystem* SignalSubsystem = UWorld::GetSubsystem<UMassSignalSubsystem>(Owner.GetWorld());
	SubscribeToSignal(*SignalSubsystem, "MyCustomName");
}
