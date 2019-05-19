// Fill out your copyright notice in the Description page of Project Settings.

//#include "SCharacter.h"
#include "Public/SCharacter.h"
#include "Public/SWeapon.h"
#include "Public/SHealthComponent.h"

#include "CoopGame.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "GameFramework/PawnMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"

//#include "GameFramework/Actor.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a  camera component
	//CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	// Attach the camera component to our capsule component.
	/*CameraComp->SetupAttachment(GetCapsuleComponent());
	CameraComp->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));*/
	
	//GetMesh()->SetSimulatePhysics(true);

	//Create a Arm Camera Comp
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;	
	SpringArmComp->SetupAttachment(RootComponent);
	
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_MYWEAPONCHANNEL, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	// Create a camera component
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	//CameraComp->SetRelativeLocation(FVector(0.0f, 50.0f, 50.0f);// +BaseEyeHeight));
	//CameraComp->SetRelativeRotation(FVector(0.0f, -20.0f, 0.0f);// +BaseEyeHeight));
	//CameraComp->bUsePawnControlRotation = true; ya heredado del espring arm
	

	ZoomedFOV = 65.0f;

	InterpSpeed = 20.0f;

	WeaponSocketName = "GunSocket";

	Ammo = 100;
	
	UE_LOG(LogTemp, Warning, TEXT("Hero Initial Ammo Load Up 100"));

	TimeReloadClip = 2.0f;

 }

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComp->FieldOfView;
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	//Spawn Default weapon only in Server
	if(Role == ROLE_Authority)
	{
		

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//SpawnParams.Owner = this;

		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (CurrentWeapon)
		{

			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName
			);

		}
	}
	



	
}

void ASCharacter::MoveForward(float Value)
{
	// Find out which way is "forward" and record that the player wants to move that way.

	//Epic wAy
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	//AddMovementInput(Direction, Value);

	//TL Way
	Direction = GetActorForwardVector(); 
	AddMovementInput(Direction * Value);
}

void ASCharacter::MoveRight(float Value)
{
	// Find out which way is "right" and record that the player wants to move that way.

	//Epic wAy
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y); 
	//AddMovementInput(Direction, Value);

	//TL Way
	Direction = GetActorRightVector();
	AddMovementInput(Direction * Value);
}

void ASCharacter::StartJump()
{
	bPressedJump = true;
}

void ASCharacter::StopJump()
{
	bPressedJump = false;
}

void ASCharacter::BeginCrouch()
{

	Crouch();
}



void ASCharacter::EndCrouch()
{
	UnCrouch();
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, InterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);


}


// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up "movement" bindings.
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	// Set up "look" bindings.
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);

	// Set up "action" bindings.
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::StopJump);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::StartZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::StopZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);

	//PlayerInputComponent->BindAction("Recargar", IE_Pressed, this, &ASCharacter::BeginReload);
	//PlayerInputComponent->BindAction("Recargar", IE_Released, this, &ASCharacter::EndReload);

}

FVector ASCharacter::GetPawnViewLocation() const
{

	if(CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
	
}

void ASCharacter::AddAmmo(int inAmmo)
{

	Ammo += inAmmo;

	if(inAmmo>=0)
	{
		inAmmo = 0;
	}


}



void ASCharacter::StartZoom()
{
	bWantsToZoom = true;
}

void ASCharacter::StopZoom()
{
	bWantsToZoom = false;
}


void ASCharacter::StartFire()
{
	

	if(CurrentWeapon!= nullptr)
	{
		/*const float ForceAmount = 20000.0f;
		GetMesh()->AddForce(FVector(0.0f, 0.0f, ForceAmount));*/			
		
		CurrentWeapon->StartFire();


	}

}

void ASCharacter::StopFire()
{	
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->StopFire();
	}
}

void ASCharacter::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if(Health<=0.0f && !bDied)
	{
		//Die...
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(10.0f);
			
		//return;
	}
}

void ASCharacter::BeginReload()
{
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->Reload(45);
	}

	/*bIsvisibleReloadBar = true;
	float TimeBetweenBarAdvance = 1 / TimeReloadClip;
	GetWorldTimerManager().SetTimer(TH_TimeToReload, this, &ASCharacter::AddTimeReload, TimeBetweenBarAdvance, false);
	*/
}



int ASCharacter::GetAmmo()
{
	return Ammo;
}

int ASCharacter::GetClip()
{
	if (CurrentWeapon != nullptr)
	{
		return CurrentWeapon->Clip;
	}
	else return 0;

	
}
//int ASCharacter::GetTimeReloadClipBar()
//{
//	
//	return TimeReloadClip;
//
//}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);

	DOREPLIFETIME(ASCharacter, bDied);

	DOREPLIFETIME(ASCharacter, Ammo);

	
}