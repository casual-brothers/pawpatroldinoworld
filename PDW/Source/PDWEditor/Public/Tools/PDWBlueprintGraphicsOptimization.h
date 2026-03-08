
#include "AssetActionUtility.h"

#include "PDWBlueprintGraphicsOptimization.generated.h"


UCLASS()
class UPDWBlueprintGraphicsOptimization : public UAssetActionUtility
{
	GENERATED_BODY()

public:

	UFUNCTION(CallInEditor)
	void BlueprintGraphicsOptimization(bool bHLOD, bool bEvaluateWPO, bool bWPOWritesVelocity, bool bUseAsOccluder, bool bAllowClothActors, bool bAffectDynamicIndirectLighting, bool bPerBoneMotionBlur, float DrawDistanceFactor = 5.0f);
};