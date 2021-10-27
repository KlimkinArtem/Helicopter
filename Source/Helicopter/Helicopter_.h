// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Helicopter_.generated.h"

UCLASS()
class HELICOPTER_API AHelicopter_ : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHelicopter_();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* Body;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* Rotor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* Blades;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* RearRotor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* RearBlade;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* Weapon;
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category="Curve")
	UCurveFloat* LiftCurve;

	

	UPROPERTY(EditAnywhere, Category="Trace")
	float Radius = 50.f;

	UPROPERTY(EditAnywhere, Category="Trace")
	float Segments = 18.f;

	UPROPERTY(EditDefaultsOnly, Category= "Destruction")
	TSubclassOf<AActor> RadialForce;

private:
	 //Константы
	 
	const float MaxBladeRotationSpeed = 1500.f;
	const float ThrottleUpSpeed = 200.f;
	const float TurnSpeed = 0.1f;
	const float BlurBladeSpeed = 900.f;

	//Просто переменные

	float TargetBladeRotationSpeed = 0;
	float CurrentBladeRotationSpeed = 0;
	float CurrentTurn = 0;
	float CurrentPitch = 0;

	FVector Velocity = FVector(0, 0, 0);
	FVector Acceleration = FVector(0, 0, 0);
	FVector PrevVelocity = FVector(0, 0, 0);
	FVector PrevAcceleration = FVector(0, 0, 0);
	FVector PrevPosition = FVector(0, 0, 0);

	//Методы
	
	void UpdateBladeRotation(float DeltaTime);
	void CalculatVelocityAndAceleration(float DeltaTime);
	void UpdatePrevVersion();
	void ApplyGravity(float DeltaTime);
	void ApplyLift(float DeltaTime);
	void SimulateForwardAcceleration(float DeltaTime);
	void MoveHelicopter(float DeltaTime);
	void RotateBlades(float DeltaTime);
	
	float GetCurrentLift() const;
	float GetTurnSpeed() const;

	//inputs
	
	void HeliThrottle(float Value);
	void HeliTurn(float Value);
	void HeliPitch(float Value);
	void Debug();
};



