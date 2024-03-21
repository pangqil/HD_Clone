// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "DrawDebugHelpers.h"
#include "TurretProjectile.h"
#include "TurretSystemFunctionLibrary.h"
#include "Components/ArrowComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundCue.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ATurret::ATurret()
{
	bReplicates=true;
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultMesh(TEXT("/Game/Skills/Turrets/Models/SM_Turret1.SM_Turret1"));

	TurretMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetSkeletalMesh(DefaultMesh.Object);
	SetRootComponent(RootComponent);
	
	RotationAC = CreateDefaultSubobject<UAudioComponent>("RotationAudioComponent");
	RotationAC->SetupAttachment(TurretMesh);
	RotationAC->bAlwaysPlay = true;

	FireAC = CreateDefaultSubobject<UAudioComponent>("FireAudioComponent");
	FireAC->SetupAttachment(TurretMesh);
	FireAC->bAlwaysPlay = true;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComponent->SetupAttachment(TurretMesh);
	ArrowComponent->SetRelativeLocation({0.f,80.f,70.f});
	
	ActorsToIgnore.Reserve(4);
	ActorsToIgnore.Add(this);

}

void ATurret::FindTarget()
{
	
	DrawDetectionRange();
	TArray<AActor*> OverlappingActors;

	const bool IsOverlapped = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(),
																		SenseRange, TraceObjectTypes, nullptr,
																		ActorIgnoreSphereOverlap, OverlappingActors);
	float BestDistance = SenseRange;

	AActor* ClosestTarget = nullptr;

	if(IsOverlapped)
	{	
		for (AActor*& HitResult : OverlappingActors)
		{
			ActorsToIgnore.Add(HitResult);

			if(!ClosestTarget || GetDistanceTo(HitResult) < BestDistance)
			{
				if(UTurretSystemFunctionLibrary::HasLineOfSight(this, SightHitResult, GetActorLocation(), HitResult->GetActorLocation(), ActorsToIgnore))
				{
					ClosestTarget = HitResult;
					BestDistance = GetDistanceTo(ClosestTarget);
				}
			}
		}

		BestTarget = ClosestTarget;
	}
	else
	{
		BestTarget = nullptr;
	}
}

void ATurret::DrawDetectionRange()
{
	if (GetWorld())
	{
		UE_LOG(LogTemp, Warning,TEXT("test"));
		DrawDebugSphere(GetWorld(), GetActorLocation(), SenseRange, 32, FColor::Red, false, -1.0f, SDPG_World);
	}
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
	// FullHealth=100.0f;
	// HealthPercentage=1.0f;
	//
	
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	if(RotationSoundCue)
	{
		RotationAC->SetSound(RotationSoundCue);
	}
	
	if(FireSoundEffect)
	{
		FireAC->SetSound(FireSoundEffect);
	}
	
	ActorsToIgnore.Add(ProjectileActor.GetDefaultObject());

	
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FindTarget();
	if(BestTarget)
	{
		RotateToTarget();
		
		FireProjectile();
	}
	else
	{
		if(GetWorld()->GetTimerManager().IsTimerActive(FireTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		}
		IdleRotate(DeltaTime);
	}
}
void ATurret::RotateToTarget()
{
	if(BestTarget && TurretMesh)
	{
		const FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(TurretMesh->GetRelativeLocation(),BestTarget->GetActorLocation());
		TurretMesh->SetRelativeRotation({TurretMesh->GetRelativeRotation().Pitch, DesiredRotation.Yaw, TurretMesh->GetRelativeRotation().Roll});
	}
}

void ATurret::PlayRotateSound()
{
	if(RotationSoundCue)
	{
		RotationAC->Stop();
		RotationAC->Play();
	}
}

void ATurret::PlayFireSound()
{
	if(FireAC)
	{
		FireAC->Stop();
		FireAC->Play();
	}
}

void ATurret::IdleRotate(const float DeltaSecond)
{
	if(!bIsRotating)
	{
		RandValue = FMath::FRandRange(-180.f,180.f);
		
		PlayRotateSound();
		bIsRotating = true;	
	}

	if(bIsRotating && !bIsInDelayTime)
	{
		RotateValue = FMath::FInterpTo(TurretMesh->GetRelativeRotation().Yaw, RandValue, DeltaSecond, InterpolationSpeed);
		TurretMesh->SetRelativeRotation({TurretMesh->GetRelativeRotation().Pitch, RotateValue, TurretMesh->GetRelativeRotation().Roll});
	}

	if(FMath::IsNearlyEqual(RandValue, TurretMesh->GetRelativeRotation().Yaw, 1.f) && !bIsInDelayTime)
	{
		bIsInDelayTime = true;
		
		if(bIsInDelayTime)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle,[&]()
			{
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
				bIsInDelayTime = false;
				bIsRotating = false;
			},1.f,false,FMath::RandRange(1.1f, 1.6f));
		}
	}
}//gpt

void ATurret::FireProjectile()
{
	ATurretProjectile* TurretProjectile = GetWorld()->SpawnActor<ATurretProjectile>(ProjectileActor.Get(),
	ArrowComponent->GetComponentLocation(), {0,TurretMesh->GetRelativeRotation().Yaw, 0});
	PlayFireSound();
}
