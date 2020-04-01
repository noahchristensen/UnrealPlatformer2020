// Noah Christensen 2020

#pragma once

#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "PlayerHUD.h"

#include "PlayerCharacter.generated.h"

UCLASS()
class ROPE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Landed(const FHitResult& Hit) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void Jump();
	void Land();

	void Dash();
	void StopDashing();
	void ResetDash();

	void MouseYaw(float axis); // x
	void MousePitch(float axis); // y
	void RotatePlayer();

	void Aim(); // aim to shoot "rope arrow"
	void UnAim(); // revert back
	void Fire(); // fire when aiming
	void ResetFire();

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void HoldRope();
	void LetGoRope();
	void SwitchMode();
	void RopeModeDissappear();
	void ExtraGrabEnd();
	void AdjustOnRope(); // calculates where and adjust where player should be (trigonometry :) )

	float CalculateDistance();

	//Vector
	FVector2D MouseInput;

	//Camera
	UPROPERTY(EditAnywhere)
		USpringArmComponent* SpringArm; // rotate camera
	UPROPERTY(EditAnywhere)
		float ArmDistance = 800.f; // to control 3rd person camera distance
	UCameraComponent* Camera3rdPerson;

	// Aiming
	bool Aiming = false;
	UCameraComponent* Camera1stPerson;
	UPROPERTY(EditAnywhere)
		float AimVelocityMod = 0.35f; // slow down when aiming
	UPROPERTY(EditAnywhere)
		float AimRange = 3000.f;

	// Shooting
	UPROPERTY(EditAnywhere)
		UClass* RopeBullet;
	FTimerHandle FireDelayTimerHandle; // shot delay
	bool CanFire = true;
	float FireDelay = 1.f;
	TArray< AActor* > RopeArray = TArray<AActor*>();
	int RopesOnScreen = 10.f;

	// Jumping
	UPROPERTY(EditAnywhere)
		float JumpHeight = 600.f;
	int JumpCount = 0;

	// Dashing
	UPROPERTY(EditAnywhere)
		float DashDistance = 6000.f;
	UPROPERTY(EditAnywhere)
		float DashCooldown = 1.f;
	UPROPERTY(EditAnywhere)
		float DashStop = 0.1f;
	FTimerHandle DashHandle;
	bool CanDash = true;

	// Rope Interacting
	bool CanClimbRope = false; // intersecting with rope collider
	bool OnRope = false; // holding on the rope
	bool Swinging = true;
	bool ExtraGrab = false; // can still grab rope if pressed too early
	UPROPERTY(EditAnywhere)
		float ExtraGrabTime = 0.5f;
	UPROPERTY(EditAnywhere)
		float ClimbSpeed = 0.5f;
	UPROPERTY(EditAnywhere)
		float SwingSpeed = 5.f;
	float PreviousValue = 0.f; // used to stop speed when climbing
	UPrimitiveComponent* CurrentRope; // box collider player is colliding with/was last colliding with
	TArray<USceneComponent*> PhysicsBall;
	AActor* RopeBase;
	float OnRopePosition;
	float StoredPitch;
	FTimerHandle ExtraGrabHandle;
	FTimerHandle RopeModeHandle;

	// Momentum
	FVector SavedVelocity;
	FVector SavedRopeVelocity;
	bool MomentumCarried = false;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* StaticMesh;

	APlayerHUD* PlayerHUD;
};
