// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/Animation/AnimMontage.h"
#include "Runtime/Engine/Classes/Animation/BlendSpace1D.h"
#include "Runtime/Engine/Classes/Animation/AnimSingleNodeInstance.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintComponent.h"
#include "Runtime/Engine/Classes/PhysicsEngine/BodySetup.h"
#include "GameFramework/Character.h"
#include "MyAnimInstance.h"
#include "MyEnemy.generated.h"

class AStake;

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

	enum class ECharacterOreintation
	{
		FRONT,
		BACK,
		LEFT,
		RIGHT
	};

	//Determines speed decrease after stake impacts mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	float StakeResistanceFactor;

	//If stake energy is bigger than this value on impact mesh will get impaled
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	float KineticEnergyThreshold;

	TArray<int16> BoneIndices;

	ECharacterOreintation MeshOrientation;

	UAnimSequence * CurrentGetUpAnim;

	float CurrentAnimPosition;

	void CheckMeshOrientation();

	//check character oreintation
	bool IsLyingOnFace(float& distance);

	bool IsLyingOnRightSide(float& distance);

	void ChooseGetUpAnimation();

	FTransform AMyEnemy::GetBoneTransformAtTime(UAnimSequence* MyAnimSequence, float AnimTime, int16 BoneIdx, bool bUseRawDataOnly);

	AStake* currStake = nullptr;

	//for state tracking
	bool bIsInactive = false;

	bool bIsImpaled = false;

	bool bIsRagdoll = false;

	bool bIsRecovering = false;

	bool bIsPlayingGetUpAnim = false;

	bool bShouldBeStill = false;

	float PhysicsAlpha;

	float inactivationTime = 0.2f;

	float Timer = 0.0f;

	FName GrabbedBone;

	UAnimMontage* Montage;

	FRotator RotationBoneAxis;
};
 
