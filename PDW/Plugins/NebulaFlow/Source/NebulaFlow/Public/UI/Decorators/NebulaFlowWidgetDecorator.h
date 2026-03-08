// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "NebulaFlowWidgetDecorator.generated.h"

class UUserWidget;
class URichTextBlock;

USTRUCT(Blueprintable, BlueprintType)
struct FNebulaRichWidgetRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WidgetClass = nullptr;

	UPROPERTY(EditAnywhere)
	FVector2D DefaultSize = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere)
	FMargin Padding;

};

UCLASS()
class NEBULAFLOW_API UNebulaFlowWidgetDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:
	UNebulaFlowWidgetDecorator(const FObjectInitializer& ObjectInitializer);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	virtual const TSubclassOf<UUserWidget> GetWidgetClass(FName TagOrId, bool bWarnIfMissing);

	virtual const FVector2D GetWidgetSize(FName TagOrId, bool bWarnIfMissing);

	virtual const FMargin GetWidgetPadding(FName TagOrId, bool bWarnIfMissing);

protected:
	FNebulaRichWidgetRow* FindWidgetRow(FName TagOrId, bool bWarnIfMissing);

	UPROPERTY(EditAnywhere, meta = (RequiredAssetDataTags = "RowStructure=/Script/NebulaFlow.NebulaRichWidgetRow"))
	TObjectPtr<class UDataTable> RichWidgetTable{};

};
