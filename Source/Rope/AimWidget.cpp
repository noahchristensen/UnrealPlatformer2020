// Noah Christensen 2020

#include "AimWidget.h"

UAimWidget::UAimWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UAimWidget::NativeConstruct() // akin to beginplay
{
	Super::NativeConstruct();
}

void UAimWidget::GenerateReticle(bool aiming)
{
	if (Reticle)
	{
		if (aiming)
		{
			Reticle->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Reticle->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UAimWidget::InRange(float value)
{
	if (Reticle)
	{
		if (value < MaxRange)
		{
			Reticle->SetColorAndOpacity(FLinearColor::Green);
		}
		else
		{
			Reticle->SetColorAndOpacity(FLinearColor::Red);
		}
	}
}

