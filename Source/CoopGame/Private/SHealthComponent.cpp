// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/SHealthComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	
	DefaultHealth = 100;

	bIsDead = false;

	SetIsReplicated(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	// Only hook if we are server
	//if(HasAuthority())
	if(GetOwnerRole() == ROLE_Authority) //Ya que es un componente
	{

		AActor* MyOwnwer = GetOwner();

		if (MyOwnwer != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("USHealthComponent found it of %s "), *MyOwnwer->GetName());

			MyOwnwer->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
		else UE_LOG(LogTemp, Error, TEXT("USHealthComponent doesn´t found Owner or is none"));

		Health = DefaultHealth;
	}
		


	//Health = DefaultHealth;
	
}

void USHealthComponent::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	
	
	if(Damage<0.0 || bIsDead)
	{
		return;
	}

	//Updated Health clamped

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	
	UE_LOG(LogTemp, Log, TEXT("Health Changed to %s "), *FString::SanitizeFloat(Health));

	if (Health <= 0.0f) bIsDead = true;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	
	
}


void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, Health);

	//DOREPLIFETIME_CONDITION(USHealthComponent, Health, COND_SkipOwner);
}




