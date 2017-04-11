// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CODETEST_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
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
