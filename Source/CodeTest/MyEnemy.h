// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "MyEnemy.generated.h"

UCLASS()
class CODETEST_API AMyEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	//Idle animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	UAnimSequence * AnimIdle;

	//Front GetUp animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	UAnimSequence * AnimFront;

	//Back GetUp animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	UAnimSequence * AnimBack;

	//Left GetUp animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	UAnimSequence * AnimLeft;

	//Right GetUp animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	UAnimSequence * AnimRight;

	bool IsLyingOnFace();

	bool bIsGrabbed = false;

	bool bIsImpaled = false;

	bool bIsRagdoll = false;

	bool bIsRecovering = false;

	bool bIsPlayingGetUpAnim = false;

	float PhysicsAlpha;

	FName GrabbedBone;
};
 
