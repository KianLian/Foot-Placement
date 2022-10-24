// Copyright Epic Games, Inc. All Rights Reserved.

#include "FootPlacementCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "UIKAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"

//////////////////////////////////////////////////////////////////////////
// AFootPlacementCharacter

AFootPlacementCharacter::AFootPlacementCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input
	
void AFootPlacementCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFootPlacementCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFootPlacementCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFootPlacementCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFootPlacementCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AFootPlacementCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AFootPlacementCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AFootPlacementCharacter::OnResetVR);
}


void AFootPlacementCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetMesh()->SkeletalMesh)
	{
		const FVector LeftFootBoneWorldLocation = GetMesh()->GetBoneLocation(LeftFootBoneName);
		LeftFootBoneRelativeLocation = GetActorTransform().InverseTransformPosition(LeftFootBoneWorldLocation);
		const FVector RightFootBoneWorldLocation = GetMesh()->GetBoneLocation(RightFootBoneName);
		RightFootBoneRelativeLocation = GetActorTransform().InverseTransformPosition(RightFootBoneWorldLocation);
		InitialMeshRelativeLocation = GetMesh()->GetRelativeTransform().GetLocation();
	}

}

void AFootPlacementCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	UUIKAnimInstance* FPAnim = Cast<UUIKAnimInstance>(GetMesh()->GetAnimInstance());
	if (!FPAnim)
		return;


	if (GetVelocity().Size() == 0)
	{
		TArray<AActor*> ActorsToIgnore;
		FHitResult LeftTraceHit;
		const FVector LeftFootLocation = GetTransform().TransformPosition(LeftFootBoneRelativeLocation);
		bool bLeftFootTraceHit = UKismetSystemLibrary::SphereTraceSingle(this, LeftFootLocation + FVector(0.f, 0.f, 50.f),
			LeftFootLocation + FVector(0.f, 0.f, -100.f), 10, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame, LeftTraceHit, true);
		FHitResult RightTraceHit;
		const FVector RightFootLocation = GetTransform().TransformPosition(RightFootBoneRelativeLocation);
		bool bRightFootTraceHit = UKismetSystemLibrary::SphereTraceSingle(this, RightFootLocation + FVector(0.f, 0.f, 50.f),
			RightFootLocation + FVector(0.f, 0.f, -100.f), 10, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame, RightTraceHit, true);


		float ZOffset = 0;

		if (LeftTraceHit.ImpactPoint.Z < RightTraceHit.ImpactPoint.Z)
		{
			ZOffset = LeftFootLocation.Z - LeftTraceHit.ImpactPoint.Z;
			FPAnim->SetRightFootEffectorLocation(RightTraceHit.ImpactPoint + FVector(0.f, 0.f, 14.f));
			FPAnim->SetLeftFootAlpha(0.f);
			FPAnim->SetRightFootAlpha(1.f);

		}
		else
		{
			ZOffset = RightFootLocation.Z - RightTraceHit.ImpactPoint.Z;
			FPAnim->SetLeftFootEffectorLocation(LeftTraceHit.ImpactPoint + FVector(0.f, 0.f, 14.f));
			FPAnim->SetLeftFootAlpha(1.f);
			FPAnim->SetRightFootAlpha(0.f);
		}
			GetMesh()->SetRelativeLocation(InitialMeshRelativeLocation + FVector(0.f, 0.f, -ZOffset + 14.f));
		}
	else
	{
		GetMesh()->SetRelativeLocation(InitialMeshRelativeLocation);
		FPAnim->SetLeftFootAlpha(0.f);
		FPAnim->SetRightFootAlpha(0.f);

	}




}

void AFootPlacementCharacter::OnResetVR()
{
	// If FootPlacement is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in FootPlacement.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AFootPlacementCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AFootPlacementCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AFootPlacementCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFootPlacementCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFootPlacementCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AFootPlacementCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
