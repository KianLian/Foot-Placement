 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UIKAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FOOTPLACEMENT_API UUIKAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UUIKAnimInstance();
	void SetLeftFootEffectorLocation(FVector NewEffectorLocation);
	void SetRightFootEffectorLocation(FVector NewEffectorLocation);
	void SetLeftFootAlpha(float NewAlpha);
	void SetRightFootAlpha(float NewAlpha);

protected:
	/** Left foot effector location */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = IK, meta = (BlueprintProtected = true))
		FVector LeftFootEffectorLocation;
	/** Joint target left */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = IK, meta = (BlueprintProtected = true))
		FVector JointTargetLeft;
	/** Left foot alpha */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = IK, meta = (BlueprintProtected = true))
		float LeftFootAlpha;
	/** Right foot effector location */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = IK, meta = (BlueprintProtected = true))
		FVector RightFootEffectorLocation;
	/** Joint target right */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = IK, meta = (BlueprintProtected = true))
		FVector JointTargetRight;
	/** Right foot alpha */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = IK, meta = (BlueprintProtected = true))
		float RightFootAlpha;
};
