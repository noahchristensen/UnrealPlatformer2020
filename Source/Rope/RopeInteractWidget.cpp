// Noah Christensen 2020


#include "RopeInteractWidget.h"

URopeInteractWidget::URopeInteractWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void URopeInteractWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void URopeInteractWidget::RopeMode(bool swinging)
{
	if (TXTMode)
	{
		if(TXTMode->Visibility == ESlateVisibility::Hidden)
		{
			TXTMode->SetVisibility(ESlateVisibility::Visible);
		}

		if (swinging)
		{
			TXTMode->SetText(FText::FromString("Switched to Swinging Mode"));
		}
		else
		{
			TXTMode->SetText(FText::FromString("Switched to Climbing Mode"));
		}
	}
}

void URopeInteractWidget::RopeModeDissappear()
{
	if (TXTMode)
	{
		TXTMode->SetVisibility(ESlateVisibility::Hidden);
	}
}

void URopeInteractWidget::GrabRope(bool colliding)
{
	if (GrabbingHand)
	{
		if (colliding)
		{
			GrabbingHand->SetText(FText::FromString("Grab"));
			GrabbingHand->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			GrabbingHand->SetText(FText::FromString(""));
			GrabbingHand->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
