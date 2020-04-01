// Noah Christensen 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "Components/WidgetComponent.h"

#include "AimWidget.h"
#include "RopeInteractWidget.h"

#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class ROPE_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	APlayerHUD();

	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
		void GenerateReticle(bool aiming);

	UFUNCTION()
		void InRange(float value);

	UFUNCTION()
		void RopeMode(bool swinging);

	UFUNCTION()
		void RopeModeDissappear();

	UFUNCTION()
		void GrabRope(bool colliding);

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> AimWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> RopeInteractWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> RopeCollideWidgetClass;

private:
	UAimWidget* AimWidget;
	URopeInteractWidget* RopeInteractWidget;
};
