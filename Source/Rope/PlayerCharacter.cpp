// Noah Christensen 2020


#include "PlayerCharacter.h"

#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

APlayerCharacter::APlayerCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 3rd Person
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera3rdPerson = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera3rdPerson"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	StaticMesh->AttachTo(RootComponent);
	SpringArm->AttachTo(RootComponent); // UPDATE FUNCTION
	SpringArm->TargetArmLength = ArmDistance; // camera 5 meters from player
	SpringArm->SetWorldRotation(FRotator(-40.f, 0.f, 0.f)); // UPDATE FUNCTION

	Camera3rdPerson->AttachTo(SpringArm, USpringArmComponent::SocketName);

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	UPrimitiveComponent* Collider = GetCapsuleComponent();
	Collider->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBegin);
	Collider->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapEnd);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	PlayerHUD->GenerateReticle(Aiming);
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	JumpCount = 0;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* 
	TODO
	- Rope
		- momentum/ carry velocity on to rope
		-Bugs
			-ropes colliding with one another(player falls off)
			-going through walls(make floor and walls different collision[no collide with floor, do collide with walls])
			-extend jump range bug
	- Dash
		- make sure works with rope
	*/

	RotatePlayer();

	if (OnRope && CurrentRope)
	{
		AdjustOnRope();
	}

	if (ExtraGrab)
	{
		HoldRope();
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MouseYaw", this, &APlayerCharacter::MouseYaw);
	InputComponent->BindAxis("MousePitch", this, &APlayerCharacter::MousePitch);

	InputComponent->BindAction("Aim", IE_Pressed, this, &APlayerCharacter::Aim);
	InputComponent->BindAction("UnAim", IE_Released, this, &APlayerCharacter::UnAim);
	InputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::Fire);

	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);

	InputComponent->BindAction("Dash", IE_Pressed, this, &APlayerCharacter::Dash);

	InputComponent->BindAction("HoldRope", IE_Pressed, this, &APlayerCharacter::HoldRope);
	InputComponent->BindAction("LetGoRope", IE_Released, this, &APlayerCharacter::LetGoRope);

	InputComponent->BindAction("SwitchMode", IE_Pressed, this, &APlayerCharacter::SwitchMode);
}

void APlayerCharacter::MouseYaw(float axis)
{
	MouseInput.X = axis;
}

void APlayerCharacter::MousePitch(float axis)
{
	MouseInput.Y = axis;
}

void APlayerCharacter::Aim()
{
	Aiming = true;
	SpringArm->TargetArmLength = 0.f; // first person

	if (PlayerHUD)
	{
		PlayerHUD->InRange(1.f);
		PlayerHUD->GenerateReticle(Aiming);
	}

}

void APlayerCharacter::UnAim()
{
	Aiming = false;
	if (PlayerHUD)
	{
		PlayerHUD->GenerateReticle(Aiming);
	}
	SpringArm->TargetArmLength = ArmDistance; // 3rd person
}

void APlayerCharacter::Fire()
{
	if (Aiming && CanFire)
	{
		CanFire = false;

		FVector PlayerViewPointLocation;
		FRotator PlayerViewPointRotation;

		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
			PlayerViewPointLocation,
			PlayerViewPointRotation
		);

		FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * AimRange;

		DrawDebugLine(
			GetWorld(),
			PlayerViewPointLocation,
			LineTraceEnd,
			FColor(0, 255, 0),
			false,
			0.f,
			5.f
		);

		FHitResult Hit;
		FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
		GetWorld()->LineTraceSingleByObjectType(
			Hit,
			PlayerViewPointLocation,
			LineTraceEnd,
			FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
			TraceParams
		);

		AActor* HitActor = Hit.GetActor();
		FVector SpawnLocation = Hit.ImpactPoint;
		FRotator SpawnRotation = Hit.ImpactNormal.Rotation();
		if (HitActor && Hit.Distance < AimRange)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit"));
			if (RopeArray.Num() >= RopesOnScreen)
			{
				GetWorld()->DestroyActor(RopeArray.Pop(), false, true);
			}
			RopeArray.Insert(this->GetWorld()->SpawnActor(RopeBullet, &SpawnLocation, &SpawnRotation, FActorSpawnParameters()), 0);
		}

		GetWorld()->GetTimerManager().SetTimer(FireDelayTimerHandle, this, &APlayerCharacter::ResetFire, FireDelay, false);
	}
}

void APlayerCharacter::ResetFire()
{
	CanFire = true;
	GetWorldTimerManager().ClearTimer(FireDelayTimerHandle);
}

void APlayerCharacter::MoveForward(float Value)
{
	if (OnRope)
	{
		if (Swinging)
		{
			if (Value > 0.1f)
			{
				FVector Direction = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X);
				CurrentRope->GetParentComponents(PhysicsBall);
				PhysicsBall.Top()->AddRelativeLocation((Direction * Value * SwingSpeed), false, nullptr, ETeleportType::None);
			}
			else if (Value < -0.1f)
			{
				FVector Direction = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X);
				CurrentRope->GetParentComponents(PhysicsBall);
				PhysicsBall.Top()->AddRelativeLocation((Direction * Value * SwingSpeed), false, nullptr, ETeleportType::None);
			}
		}
		else // Climbing
		{
			if (Value > 0.1f)
			{
				FVector RopeDirection = FRotationMatrix(CurrentRope->GetComponentRotation()).GetScaledAxis(EAxis::X);
				//FVector Direction = FVector(0, 0, ClimbSpeed);

				if (Aiming)
				{
					Value = Value * AimVelocityMod;
				}
				AddMovementInput(RopeDirection, Value * ClimbSpeed);
			}
			else if (Value < -0.1f)
			{
				FVector RopeDirection = FRotationMatrix(CurrentRope->GetComponentRotation()).GetScaledAxis(EAxis::X);

				if (Aiming)
				{
					Value = Value * AimVelocityMod;
				}
				AddMovementInput(RopeDirection, Value * ClimbSpeed);
			}
			else if((Value < 0.1f && Value > -0.1f) && (PreviousValue > 0.1f || PreviousValue < -0.1f))
			{
				GetCharacterMovement()->SetMovementMode(MOVE_None);
				GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			}

			PreviousValue = Value;
		}
	}
	else
	{
		if (Value > 0.1f)
		{
			FVector Direction = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X); // grab rotation so we know what relative forward is

			if (Aiming)
			{
				Value = Value * AimVelocityMod;
			}
			AddMovementInput(Direction, Value);
		}
		if (Value < -0.1f)
		{
			FVector Direction = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X); // grab rotation so we know what relative forward is

			if (Aiming)
			{
				Value = Value * AimVelocityMod;
			}
			AddMovementInput(Direction, Value);
		}
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (OnRope)
	{
		// swing to player right and left?
	}
	else
	{
		if (Value > 0.1f)
		{
			FVector Direction = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::Y);

			if (Aiming)
			{
				Value = Value * AimVelocityMod;
			}
			AddMovementInput(Direction, Value);
		}
		else if (Value < -0.1f)
		{
			FVector Direction = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::Y);

			if (Aiming)
			{
				Value = Value * AimVelocityMod;
			}
			AddMovementInput(Direction, Value);
		}
	}
}

void APlayerCharacter::Jump()
{
	if (JumpCount == 0)
	{
		if (CanClimbRope)
		{
			UE_LOG(LogTemp, Warning, TEXT("Jumped Off Rope"));
			if (GetCharacterMovement()->IsFlying() == true)
			{
				GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				OnRope = false;
			}

			GetCharacterMovement()->Velocity = PhysicsBall.Top()->GetComponentVelocity();

			FVector Direction = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X);
			APlayerCharacter::LaunchCharacter(FVector(Direction.X * 1.5, Direction.Y * 1.5, JumpHeight), false, true);
			JumpCount = 1;
		}
		else
		{
			APlayerCharacter::LaunchCharacter(FVector(0, 0, JumpHeight), false, true);
			JumpCount = 1;
		}
	}
	// if(jumpCount == 1 || (jumpCount == 0 && inAir)) {Double Jump}
}

void APlayerCharacter::Dash()
{
	if (CanDash)
	{
		SavedVelocity = FVector(this->GetVelocity().X, this->GetVelocity().Y, 0);
		UE_LOG(LogTemp, Warning, TEXT("Dash"));
		GetCharacterMovement()->BrakingFrictionFactor = 0.f;
		LaunchCharacter(FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X) * DashDistance, true, true);
		CanDash = false;
		GetWorldTimerManager().SetTimer(DashHandle, this, &APlayerCharacter::StopDashing, DashStop, false);
	}
}

void APlayerCharacter::StopDashing() // For dash duration
{
	if (!OnRope)
	{
		GetCharacterMovement()->Velocity = SavedVelocity;
		GetWorldTimerManager().SetTimer(DashHandle, this, &APlayerCharacter::ResetDash, DashCooldown, false);
		GetCharacterMovement()->BrakingFrictionFactor = 2.f;
	}
	else
	{
		GetWorldTimerManager().SetTimer(DashHandle, this, &APlayerCharacter::ResetDash, DashCooldown, false);
		GetCharacterMovement()->BrakingFrictionFactor = 2.f;
	}
}

void APlayerCharacter::ResetDash() // For dash cooldown
{
	CanDash = true;
}

void APlayerCharacter::SwitchMode() // switch between climbing and swinging on rope
{
	Swinging = !Swinging;

	if (PlayerHUD)
	{
		PlayerHUD->RopeMode(Swinging);
	}

	if (!GetWorld()->GetTimerManager().IsTimerActive(RopeModeHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(RopeModeHandle, this, &APlayerCharacter::RopeModeDissappear, 2.f, false);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(RopeModeHandle);
		GetWorld()->GetTimerManager().SetTimer(RopeModeHandle, this, &APlayerCharacter::RopeModeDissappear, 2.f, false);
	}
}

void APlayerCharacter::RopeModeDissappear() // get rid of mode switch text from HUD
{
	if (PlayerHUD)
	{
		PlayerHUD->RopeModeDissappear();
	}
}

void APlayerCharacter::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) // collide with rope
{
	CurrentRope = OtherComp; // Collider for rope
	RopeBase = CurrentRope->GetOwner(); //Entirety of the rope BP(used to get position of the "top" of the rope)
	CurrentRope->GetParentComponents(PhysicsBall); // Sphere at the end the rope that gives 'rope' physics

	CanClimbRope = true;
	if (PlayerHUD)
	{
		PlayerHUD->GrabRope(CanClimbRope);
	}
}

void APlayerCharacter::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Exit"));

	CanClimbRope = false;
	OnRope = false;

	if (GetCharacterMovement()->IsFlying() == true)
	{
		// If we are not flying already then set our movement mode so that we are  
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	if (PlayerHUD)
	{
		PlayerHUD->GrabRope(CanClimbRope);
	}
}

void APlayerCharacter::HoldRope()
{
	if (CanClimbRope)
	{
		ExtraGrab = false;

		SavedRopeVelocity = this->GetVelocity();
		UE_LOG(LogTemp, Warning, TEXT("Hold Rope, velocity: %f, %f, %"), SavedRopeVelocity.X, SavedRopeVelocity.Y, SavedRopeVelocity.Z);

		if (GetCharacterMovement()->IsFlying() == false)
		{	
			GetCharacterMovement()->SetMovementMode(MOVE_None);
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			OnRope = true;
			JumpCount = 0; // Jump off ropes
		}

		//PhysicsBall.Top()->AddRelativeLocation(FVector(SavedRopeVelocity.X / 500.f, SavedRopeVelocity.Y / 500.f, 0.f), false, nullptr, ETeleportType::None);
		//PhysicsBall.Top()->Velocity
		//GetCharacterMovement()->Velocity = PhysicsBall.Top()->GetComponentVelocity();

		//MomentumCarried = true;

		AdjustOnRope();

		if (PlayerHUD)
		{
			PlayerHUD->GrabRope(false);
		}
	}
	else if(ExtraGrab == false)
	{
		ExtraGrab = true;
		GetWorld()->GetTimerManager().SetTimer(ExtraGrabHandle, this, &APlayerCharacter::ExtraGrabEnd, ExtraGrabTime, false);
	}
}

void APlayerCharacter::ExtraGrabEnd()
{
	ExtraGrab = false;
}

void APlayerCharacter::LetGoRope()
{
	if (CanClimbRope)
	{
		if (GetCharacterMovement()->IsFlying() == true)
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			OnRope = false;
		}

		if (PhysicsBall.Top())
		{
			GetCharacterMovement()->Velocity = PhysicsBall.Top()->GetComponentVelocity();
		}

		PlayerHUD->GrabRope(CanClimbRope);
	}
}

void APlayerCharacter::RotatePlayer()
{
	FRotator NewYaw = GetActorRotation();
	NewYaw.Yaw += MouseInput.X;
	SetActorRotation(NewYaw);

	FRotator NewPitch = SpringArm->GetComponentRotation();
	if (Aiming)
	{
		NewPitch.Pitch = FMath::Clamp(NewPitch.Pitch + MouseInput.Y, -80.f, 85.f);

		if (PlayerHUD)
		{
			PlayerHUD->InRange(CalculateDistance());
		}
	}
	else
	{
		NewPitch.Pitch = FMath::Clamp(NewPitch.Pitch + MouseInput.Y, -80.f, 0.f); // new y rotation must be inbetween min/max
	}

	SpringArm->SetWorldRotation(NewPitch);
}

void APlayerCharacter::AdjustOnRope()
{
	/*if (((PhysicsBall.Top()->ComponentVelocity.X * SavedRopeVelocity.X) >= 0) && MomentumCarried)
	{
		PhysicsBall.Top()->AddRelativeLocation(FVector(SavedRopeVelocity.X / 500.f, SavedRopeVelocity.Y / 500.f, 0.f), false, nullptr, ETeleportType::None);
	}
	else
	{
		MomentumCarried = false;
	}*/

	float ChangeInX = this->GetActorLocation().X - RopeBase->GetActorLocation().X;
	float ChangeInY = this->GetActorLocation().Y - RopeBase->GetActorLocation().Y;
	float ChangeInZ = this->GetActorLocation().Z - RopeBase->GetActorLocation().Z;
	if (abs(ChangeInX) > abs(ChangeInY)) // Change to Combination of both X and Y for better accuracy
	{
		OnRopePosition = sqrt(pow(ChangeInX, 2) + pow(ChangeInZ, 2)); // Hypotenuse
	}
	else
	{
		OnRopePosition = sqrt(pow(ChangeInY, 2) + pow(ChangeInZ, 2)); // Hypotenuse
	}

	float AnglePitch = -atan((PhysicsBall.Top()->GetComponentLocation().X - RopeBase->GetActorLocation().X) / (PhysicsBall.Top()->GetComponentLocation().Z - RopeBase->GetActorLocation().Z)); // theta in radians
	float XToAdd = OnRopePosition * sin(AnglePitch);

	float AngleRoll = -atan((PhysicsBall.Top()->GetComponentLocation().Y - RopeBase->GetActorLocation().Y) / (PhysicsBall.Top()->GetComponentLocation().Z - RopeBase->GetActorLocation().Z));
	float YToAdd = OnRopePosition * sin(AngleRoll);

	float ZToAdd = 0;
	if (abs(AnglePitch) > abs(AngleRoll))
	{
		ZToAdd = (1200.f - (OnRopePosition * cos(AnglePitch))); // Change to Combination of both angles for better accuracy
	}
	else
	{
		ZToAdd = (1200.f - (OnRopePosition * cos(AngleRoll)));
	}

	UE_LOG(LogTemp, Warning, TEXT("rope tick, velocity: %f, %f, %"), PhysicsBall.Top()->GetComponentVelocity().X, PhysicsBall.Top()->GetComponentVelocity().Y, PhysicsBall.Top()->GetComponentVelocity().Z);

	this->SetActorLocation(FVector((RopeBase->GetActorLocation().X + XToAdd), RopeBase->GetActorLocation().Y + YToAdd, (RopeBase->GetActorLocation().Z - 1200.f) + ZToAdd)); // this->GetActorLocation().Z
}

float APlayerCharacter::CalculateDistance()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		PlayerViewPointLocation,
		PlayerViewPointRotation
	);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * AimRange;

	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(0, 255, 0),
		false,
		0.f,
		5.f
	);

	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		TraceParams
	);

	AActor* HitActor = Hit.GetActor();
	float DistanceAway = 3001.f;

	if (HitActor)
	{
		DistanceAway = Hit.Distance;
	}

	return DistanceAway;
}


