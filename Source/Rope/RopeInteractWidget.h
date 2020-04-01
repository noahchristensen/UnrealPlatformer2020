// Noah Christensen 2020

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Styling/SlateColor.h"
#include "Math/Color.h"
#include "Components/TextBlock.h"
#include "Runtime/UMG/Public/UMG.h"

#include "RopeInteractWidget.generated.h"

/**
 * 
 */
UCLASS()
class ROPE_API URopeInteractWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URopeInteractWidget(const FObjectInitializer& ObjectIntializer);

	virtual void NativeConstruct() override;

	void RopeMode(bool swinging);

	void RopeModeDissappear();

	void GrabRope(bool colliding);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TXTMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* GrabbingHand;
};
