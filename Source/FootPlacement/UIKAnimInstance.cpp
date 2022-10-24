// Fill out your copyright notice in the Description page of Project Settings.


#include "UIKAnimInstance.h"

UUIKAnimInstance::UUIKAnimInstance()
{
	JointTargetLeft = FVector(50.000000f, 217.502960f, -38.598057f);
	JointTargetRight = FVector(50.000000f, 217.000000f, 38.000000f);
}

void UUIKAnimInstance::SetLeftFootEffectorLocation(FVector NewEffectorLocation)
{
	LeftFootEffectorLocation = NewEffectorLocation;
}

void UUIKAnimInstance::SetRightFootEffectorLocation(FVector NewEffectorLocation)
{
	RightFootEffectorLocation = NewEffectorLocation;
}

void UUIKAnimInstance::SetLeftFootAlpha(float NewAlpha)
{
	LeftFootAlpha = NewAlpha;
}

void UUIKAnimInstance::SetRightFootAlpha(float NewAlpha)
{
	RightFootAlpha = NewAlpha;
}
