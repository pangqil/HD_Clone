#pragma once

#include "CoreMinimal.h"
#include "Turret.h"
#include "AirTurret.generated.h"

/**
 * 
 */
UCLASS()
class HD_CLONE_API AAirTurret : public ATurret
{
	GENERATED_BODY()
	
private:
	bool bIsInDelayTime = false;
	
};









