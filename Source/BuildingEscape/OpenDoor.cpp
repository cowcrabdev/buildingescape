// Copyright CowCrab

#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "OpenDoor.h"



// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	TargetYaw += InitialYaw;

	TestForPlate();

	FindAudioComponent();
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is missing audio component"), *GetOwner()->GetName());
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(TotalMassOfActors() > 50.f)
	{
		OpenDoor(DeltaTime);
		// DoorLastOpened = When the door was opened
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}else 
	{
		//if door open longer than x seconds then DoorCloseDelay
		  
		if(GetWorld()->GetTimeSeconds() - DoorLastOpened >= DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	CurrentYaw = FMath::Lerp(CurrentYaw, TargetYaw, DeltaTime * DoorOpenSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	CloseDoorSound = false;
	if(!AudioComponent) {return;}
	if(!OpenDoorSound)
	{
		AudioComponent->Play();
		OpenDoorSound = true;
	}
}


void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, DeltaTime * DoorCloseSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	OpenDoorSound = false;
	if(!AudioComponent) {return;}
	if(!CloseDoorSound)
	{
		AudioComponent->Play();
		CloseDoorSound = true;
	}
}


float UOpenDoor::TotalMassOfActors() const 
{
	float TotalMass = 0.f;

	//Find all overlapping actors
	TArray<AActor*> OverLappingActors;
	if(!PressurePlate) {return TotalMass;}
	PressurePlate->GetOverlappingActors(OUT OverLappingActors);
	//Add up t heir masses
	for(AActor* Actor : OverLappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();

	}


	return TotalMass;

}

void UOpenDoor::TestForPlate()
{
	if(!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the open door component on it, but no pressureplate set!"), *GetOwner()->GetName());
	}
}