// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"


#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 3rd Person
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera3rdPerson = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera3rdPerson"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	//RootComponent = Body; // whats being rotated around, in order to use spring arm

	SpringArm->AttachTo(RootComponent); // UPDATE FUNCTION
	SpringArm->TargetArmLength = ArmDistance; // camera 5 meters from player
	SpringArm->SetWorldRotation(FRotator(-40.f, 0.f, 0.f)); // UPDATE FUNCTION

	Camera3rdPerson->AttachTo(SpringArm, USpringArmComponent::SocketName);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator NewYaw = GetActorRotation();
	NewYaw.Yaw += MouseInput.X;
	SetActorRotation(NewYaw);

	FRotator NewPitch = SpringArm->GetComponentRotation();
	if (Aiming)
	{
		NewPitch.Pitch = FMath::Clamp(NewPitch.Pitch + MouseInput.Y, -80.f, 85.f);
	}
	else
	{
		NewPitch.Pitch = FMath::Clamp(NewPitch.Pitch + MouseInput.Y, -80.f, 0.f); // new y rotation must be inbetween min/max
	}

	SpringArm->SetWorldRotation(NewPitch);
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MouseYaw", this, &APlayerPawn::MouseYaw);
	InputComponent->BindAxis("MousePitch", this, &APlayerPawn::MousePitch);

	InputComponent->BindAction("Aim", IE_Pressed, this, &APlayerPawn::Aim);
	InputComponent->BindAction("UnAim", IE_Released, this, &APlayerPawn::UnAim);

	InputComponent->BindAxis("MoveForward", this, &APlayerPawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APlayerPawn::MoveRight);
}

void APlayerPawn::MouseYaw(float axis)
{
	MouseInput.X = axis;
}

void APlayerPawn::MousePitch(float axis)
{
	MouseInput.Y = axis;
}

void APlayerPawn::Aim()
{
	UE_LOG(LogTemp, Warning, TEXT("Aim"));
	Aiming = true;
	SpringArm->TargetArmLength = 0.f; // first person
}

void APlayerPawn::UnAim()
{
	UE_LOG(LogTemp, Warning, TEXT("UnAim"));
	Aiming = false;
	SpringArm->TargetArmLength = ArmDistance; // 3rd person
}

void APlayerPawn::MoveForward(float Value)
{
	if (Value > 0.1f)
	{
		UE_LOG(LogTemp, Warning, TEXT("forward"));
		FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X); // grab rotation so we know what relative forward is
		AddMovementInput(Direction, Value, true);
	}
}

void APlayerPawn::MoveRight(float Value)
{
	if (Value > 0.1f)
	{
		UE_LOG(LogTemp, Warning, TEXT("right"));
		FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Value, true);
	}
}
