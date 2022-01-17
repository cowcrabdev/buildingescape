// Copyright CowCrab

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Grabber.h"
#include "GameFramework/PlayerController.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#define OUT


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();

	SetupInputComponent();
	
}

//event Handler for grab key
void UGrabber::Grab()
{
	
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();

	if(ActorHit)
	{
		if(!PhysicsHandle) {return;}
		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			GetView()
		);
	}
}

//event Handler for release key
void UGrabber::Release()
{

	if(!PhysicsHandle) {return;}
	PhysicsHandle->ReleaseComponent();
	

}

void UGrabber::FindPhysicsHandle(){
	
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	
	if(!PhysicsHandle){
		UE_LOG(LogTemp, Error, TEXT("Component %s does not have a physics handle"), *GetOwner()->GetName());
	}

}

void UGrabber::SetupInputComponent(){
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if(InputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Input Component %s found"), *GetOwner()->GetName());
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Release",IE_Released, this, &UGrabber::Release);
	}else {
		UE_LOG(LogTemp, Warning, TEXT("Input Component for %s missing!"), *GetOwner()->GetName());
	}
}

FVector UGrabber::GetView()
{
	//This Function returns the vector of the view + reach
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	return LineTraceEnd;
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!PhysicsHandle) {return;}
	PhysicsHandle->SetTargetLocation(GetView());
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	
	// Get PLayer viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	FHitResult Hit;
	//Ray-cast out to a certain distance (reach)
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	bool ActorWasHit = GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	AActor* HitObject = Hit.GetActor();
	// Logging out to Test
	if(ActorWasHit && HitObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Object is %s"), *HitObject->GetName())
	}
	
	return Hit;
}