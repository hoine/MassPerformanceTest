// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassProcessor.h"
#include "FindRandomPointProcessor.generated.h"

USTRUCT()
struct FFindRandomPointFragment : public FMassConstSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Radius = 1000.f;
};

USTRUCT()
struct FMassFindRandomTag : public FMassTag
{
	GENERATED_BODY()
	
};

UCLASS()
class UFindRandomPointTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
	
	UPROPERTY(EditAnywhere)
	FFindRandomPointFragment RandomPointFragment;
};

class UNavMeshPathSubsystem;
UCLASS()
class AIPERFORMANCECOMPARATION_API UFindRandomPointProcessor: public UMassProcessor
{
	GENERATED_BODY()

public:
	UFindRandomPointProcessor();
	void Initialize(UObject& Owner);

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;

	UPROPERTY()
	TObjectPtr<UNavMeshPathSubsystem> MassNavSubsystem;
};