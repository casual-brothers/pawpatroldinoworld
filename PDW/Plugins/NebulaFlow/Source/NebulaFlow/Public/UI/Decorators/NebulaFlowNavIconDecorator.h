// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "NebulaFlowNavIconDecorator.generated.h"

class UInputAction;
class URichTextBlock;
class UNebulaFlowNavigationIcon;

USTRUCT(Blueprintable, BlueprintType)
struct FNebulaRichNavigationIconRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UNebulaFlowNavigationIcon> NavIconClass = nullptr;

	UPROPERTY(EditAnywhere)
	FVector2D DefaultSize = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere)
	FMargin Padding;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> Action = nullptr;

};

UCLASS()
class NEBULAFLOW_API UNebulaFlowNavIconDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:
	UNebulaFlowNavIconDecorator(const FObjectInitializer& ObjectInitializer);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	virtual const TSubclassOf<UNebulaFlowNavigationIcon> GetNavIconClass(FName TagOrId, bool bWarnIfMissing);

	virtual const FVector2D GetNavIconSize(FName TagOrId, bool bWarnIfMissing);

	virtual const UInputAction* GetNavIconAction(FName TagOrId, bool bWarnIfMissing);

	virtual const FMargin GetNavIconPadding(FName TagOrId, bool bWarnIfMissing);

protected:
	FNebulaRichNavigationIconRow* FindNavIconRow(FName TagOrId, bool bWarnIfMissing);

	UPROPERTY(EditAnywhere, meta = (RequiredAssetDataTags = "RowStructure=/Script/NebulaFlow.NebulaRichNavigationIconRow"))
	TObjectPtr<class UDataTable> RichNavIconTable{};

};
