// Noah Christensen 2020
#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Styling/SlateColor.h"
#include "Math/Color.h"
#include "Components/TextBlock.h"
#include "Runtime/UMG/Public/UMG.h"

#include "AimWidget.generated.h"

/**
 * 
 */
UCLASS()
class ROPE_API UAimWidget : public UUserWidget
{
	GENERATED_BODY()
    
public:
    UAimWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;

    void GenerateReticle(bool aiming);

    void InRange(float value);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
        class UTextBlock* Reticle;

    UPROPERTY(EditAnywhere)
        float MaxRange = 3000.f;
};
