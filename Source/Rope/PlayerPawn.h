// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

UCLASS()
class ROPE_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void Forward();

	void MoveRight(float Value);
	void Right();

	void MouseYaw(float axis); // x
	void MousePitch(float axis); // y

	void Aim(); // aim to shoot "rope arrow"
	void UnAim(); // revert back

	//Vector
	FVector2D MouseInput;

	UPROPERTY(EditAnywhere)
		USpringArmComponent* SpringArm; // rotate camera
	UPROPERTY(EditAnywhere)
		float ArmDistance = 800.f; // to control 3rd person camera distance
	UCameraComponent* Camera3rdPerson;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
		USceneComponent* Body;

	bool Aiming = false;
	UCameraComponent* Camera1stPerson;
};
