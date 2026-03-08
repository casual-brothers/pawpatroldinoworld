// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "PDWRichTextBlockWidgetDecorator.generated.h"

class UInputAction;
class URichTextBlock;
class UNebulaFlowNavigationIcon;
class UPDWRichTextBlockWidgetDecorator;

USTRUCT(Blueprintable, BlueprintType)
struct FRichNavigationIconRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UNebulaFlowNavigationIcon> NavIconClass = nullptr;

	UPROPERTY(EditAnywhere)
	FVector2D DefaultSize = FVector2D::ZeroVector;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> Action = nullptr;
	
	UPROPERTY(EditAnywhere)
	FMargin Padding;

};

UCLASS()
class PDW_API UPDWRichTextBlockWidgetDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:
	UPDWRichTextBlockWidgetDecorator(const FObjectInitializer& ObjectInitializer);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	virtual const TSubclassOf<UNebulaFlowNavigationIcon> GetNavIconClass(FName TagOrId, bool bWarnIfMissing);

	virtual const FVector2D GetNavIconSize(FName TagOrId, bool bWarnIfMissing);

	virtual const UInputAction* GetNavIconAction(FName TagOrId, bool bWarnIfMissing);

	virtual const FMargin GetNavIconPadding(FName TagOrId, bool bWarnIfMissing);

protected:
	FRichNavigationIconRow* FindNavIconRow(FName TagOrId, bool bWarnIfMissing);

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UDataTable> RichNavIconTable {};

};
