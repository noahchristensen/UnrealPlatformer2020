// Noah Christensen 2020


#include "PlayerHUD.h"

APlayerHUD::APlayerHUD()
{

}

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	if (AimWidgetClass) // instantiate widget
	{
		AimWidget = CreateWidget<UAimWidget>(GetWorld(), AimWidgetClass);
		if (AimWidget)
		{
			AimWidget->AddToViewport();
		}
	}

	if (RopeInteractWidgetClass) // instantiate widget
	{
		RopeInteractWidget = CreateWidget<URopeInteractWidget>(GetWorld(), RopeInteractWidgetClass);
		if (RopeInteractWidget)
		{
			RopeInteractWidget->AddToViewport();
		}
	}
}

void APlayerHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void APlayerHUD::DrawHUD()
{
	Super::DrawHUD();
}

void APlayerHUD::GenerateReticle(bool aiming)
{
	if (AimWidget)
	{
		AimWidget->GenerateReticle(aiming);
	}
}

void APlayerHUD::InRange(float value)
{
	if (AimWidget)
	{
		AimWidget->InRange(value);
	}
}

void APlayerHUD::RopeMode(bool swinging)
{
	if (RopeInteractWidget)
	{
		RopeInteractWidget->RopeMode(swinging);
	}
}

void APlayerHUD::RopeModeDissappear()
{
	if (RopeInteractWidget)
	{
		RopeInteractWidget->RopeModeDissappear();
	}
}

void APlayerHUD::GrabRope(bool colliding)
{
	if (RopeInteractWidget)
	{
		RopeInteractWidget->GrabRope(colliding);
	}
}
