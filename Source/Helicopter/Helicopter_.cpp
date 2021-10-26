// Fill out your copyright notice in the Description page of Project Settings.


#include "Helicopter_.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Misc/OutputDeviceNull.h"

// Sets default values
AHelicopter_::AHelicopter_()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	RootComponent = Body;

	Rotor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rotor"));
	Rotor->SetupAttachment(Body);
	
	Blades = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Blades"));
	Blades->SetupAttachment(Rotor);

	RearRotor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RearRotor"));
	RearRotor->SetupAttachment(Body);

	RearBlade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RearBlade"));
	RearBlade->SetupAttachment(RearRotor);

	/*
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(Body);
	CameraBoom->TargetArmLength = 1300.0f;	
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraLagSpeed = 5.f;
	CameraBoom->CameraRotationLagSpeed = 5.f;
	CameraBoom->SetRelativeLocation(FVector(0,0,220));
	CameraBoom->SocketOffset = FVector(0,0,500);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = true;

	*/
}

// Called when the game starts or when spawned
void AHelicopter_::BeginPlay()
{
	Super::BeginPlay();
	UpdatePrevVersion();
}

// Called every frame
void AHelicopter_::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateBladeRotation(DeltaTime);
	CalculatVelocityAndAceleration(DeltaTime);
	UpdatePrevVersion();
	ApplyGravity(DeltaTime);
	ApplyLift(DeltaTime);
	SimulateForwardAcceleration(DeltaTime);
	MoveHelicopter(DeltaTime);
	RotateBlades(DeltaTime);
	
}

// Called to bind functionality to input
void AHelicopter_::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("HeliThrottle", this, &AHelicopter_::HeliThrottle);
	PlayerInputComponent->BindAxis("HeliTurn", this, &AHelicopter_::HeliTurn);
	PlayerInputComponent->BindAxis("HeliPitch", this, &AHelicopter_::HeliPitch);
}

void AHelicopter_::UpdateBladeRotation(float DeltaTime)
{
	CurrentBladeRotationSpeed = FMath::FInterpTo(CurrentBladeRotationSpeed, TargetBladeRotationSpeed, DeltaTime, 1.0f);
}

void AHelicopter_::CalculatVelocityAndAceleration(float DeltaTime)
{
	Velocity = (GetActorLocation() - PrevPosition) * DeltaTime;
	Acceleration = (Velocity - PrevVelocity) * DeltaTime;
}

void AHelicopter_::UpdatePrevVersion()
{
	PrevPosition = GetActorLocation();
	PrevVelocity = Velocity;
	PrevAcceleration = FVector(0, 0, 0);
}

void AHelicopter_::ApplyGravity(float DeltaTime)
{
	Acceleration = Acceleration + FVector(0, 0, -981);
}

void AHelicopter_::ApplyLift(float DeltaTime)
{
	Acceleration = (GetActorUpVector() * GetCurrentLift()) + Acceleration;
}

void AHelicopter_::SimulateForwardAcceleration(float DeltaTime)
{
	
	Acceleration = (GetActorUpVector() * (GetCurrentLift() * 2)) + Acceleration;
}

void AHelicopter_::MoveHelicopter(float DeltaTime)
{
	FVector DeltaLocation = (DeltaTime * Acceleration) + Velocity;
	AddActorWorldOffset(DeltaLocation, true);
}

void AHelicopter_::RotateBlades(float DeltaTime)
{
	FOutputDeviceNull ar;
	
	if(CurrentBladeRotationSpeed > BlurBladeSpeed)
	{
		this->CallFunctionByNameWithArguments(TEXT("SetBladeBlur"), ar, NULL, true);
		float DeltaRotationBlur = BlurBladeSpeed * DeltaTime;
		Rotor->AddRelativeRotation(FRotator(0, DeltaRotationBlur, 0));
		RearRotor->AddLocalRotation(FRotator(DeltaRotationBlur * 2, 0, 0));
		
	}else
	{
		this->CallFunctionByNameWithArguments(TEXT("SetBladeBlurNormal"), ar, NULL, true);
		float DeltaRotationNormal = CurrentBladeRotationSpeed * DeltaTime;
		Rotor->AddRelativeRotation(FRotator(0, DeltaRotationNormal, 0));
		RearRotor->AddLocalRotation(FRotator(DeltaRotationNormal * 2, 0, 0));
	}
}


float AHelicopter_::GetCurrentLift() const
{
	return LiftCurve->GetFloatValue(CurrentBladeRotationSpeed);
}

float AHelicopter_::GetTurnSpeed() const
{
	return TurnSpeed * FMath::GetMappedRangeValueClamped(FVector2D(0, 400),FVector2D(0, 1), GetCurrentLift());
	
}

void AHelicopter_::HeliThrottle(float Value)
{
	float XValue = (Value * GetWorld()->GetDeltaSeconds() * ThrottleUpSpeed) + TargetBladeRotationSpeed;
	TargetBladeRotationSpeed = FMath::Clamp(XValue, 0.f, MaxBladeRotationSpeed);
}

void AHelicopter_::HeliTurn(float Value)
{
	CurrentTurn = FMath::FInterpTo(CurrentTurn, Value * GetTurnSpeed(), GetWorld()->GetDeltaSeconds(), 2.0f);
	this->AddActorWorldRotation(FRotator(0,CurrentTurn,0));
	this->SetActorRotation(FRotator(GetActorRotation().Pitch,GetActorRotation().Yaw,CurrentTurn * 40));
}

void AHelicopter_::HeliPitch(float Value)
{
	CurrentPitch = FMath::FInterpTo(CurrentPitch, Value * GetTurnSpeed(), GetWorld()->GetDeltaSeconds(), 2.0f);
	
	if(CurrentPitch + GetActorRotation().Pitch >= -89.f && CurrentPitch + GetActorRotation().Pitch <= 89.f)
	{
		AddActorLocalRotation(FRotator(CurrentPitch,0,0));
	}
	
}