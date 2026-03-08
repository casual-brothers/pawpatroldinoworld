#pragma once

#include "NebulaGraphicsEnums.generated.h"

UENUM()
enum class ENebulaGraphicsCustomizationType : uint8
{
	StaticMesh,
	SkeletalMesh,
	Animation,
	MaterialInstance,
	MaterialInstanceDynamic,
	DecalMaterialInstance,
	DecalMaterialInstanceDynamic,
	Groom,
	Unknown
};

UENUM(BlueprintType)
enum class ENebulaGraphicsMaterialParameterType : uint8
{
	Scalar,
	Vector,
	Texture,
	Unknown
};