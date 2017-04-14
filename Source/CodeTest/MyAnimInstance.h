// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "MyEnemy.h"
#include "MyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CODETEST_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsRagdoll;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsRecovering;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float physicsBlendWeigth;

	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties")
		void UpdateAnimationProperties();

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
	
	
};
