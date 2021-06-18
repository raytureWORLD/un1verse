// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "un1verse_ntwrk_dmoHUD.generated.h"

UCLASS()
class Aun1verse_ntwrk_dmoHUD : public AHUD
{
	GENERATED_BODY()

public:
	Aun1verse_ntwrk_dmoHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

