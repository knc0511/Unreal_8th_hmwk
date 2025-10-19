#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Blueprint/UserWidget.h"
#include "BuffInfo.h"
#include "InteractableItem.h"

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	MaxHealth = 100.0f;
	Health = MaxHealth;

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	bSprinting = false;
	SpeedDebuffStack = 0;
	CurrentSpeedMultiplier = 1.0f;
	ReverseControlStack = 0;
	bIsControlReversed = false;
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverheadHP();
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Move
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopSprint
				);
			}

			if (PlayerController->InteractAction)
			{
				EnhancedInput->BindAction(
					PlayerController->InteractAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::TryInteract
				);
			}
		}
	}
}

void ASpartaCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller)
	{
		return;
	}

	FVector2D MoveInput = Value.Get<FVector2D>();

	if (bIsControlReversed)
	{
		MoveInput *= -1.f;
	}

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void ASpartaCharacter::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		Jump();
	}
}

void ASpartaCharacter::StopJump(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		StopJumping();
	}
}

void ASpartaCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& Value)
{
	bSprinting = true;
	UpdateCharacterSpeed();
}

void ASpartaCharacter::StopSprint(const FInputActionValue& Value)
{
	bSprinting = false;
	UpdateCharacterSpeed();
}

void ASpartaCharacter::TryInteract(const FInputActionValue& Value)
{
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 100.f;

	float SphereRadius = 50.f;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(SphereRadius);
	TArray<FHitResult> HitResults;

	bool HitDetected = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		Sphere,
		Params
	);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 2.0f);
	DrawDebugSphere(GetWorld(), Start, SphereRadius, 16, FColor::Blue, false, 2.0f);
	DrawDebugSphere(GetWorld(), End, SphereRadius, 16, FColor::Red, false, 2.0f);

	if (HitDetected)
	{
		for (const FHitResult& Hit : HitResults)
		{
			if (AInteractableItem* Item = Cast<AInteractableItem>(Hit.GetActor()))
			{
				Item->OnInteract();
			}
		}
	}
}

float ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UpdateOverheadHP();
}

float ASpartaCharacter::TakeDamage(
	float DamageAmount,
	struct FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UpdateOverheadHP();

	if (Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
	if (ASpartaGameState* GameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr)
	{
		GameState->OnGameOver();
	}
}

void ASpartaCharacter::UpdateOverheadHP()
{
	if (UUserWidget* WidgetInstance = OverheadWidget->GetUserWidgetObject())
	{
		if (UProgressBar* HPBar = Cast<UProgressBar>(WidgetInstance->GetWidgetFromName(TEXT("HealthBar"))))
		{
			const float HPPercent = (MaxHealth > 0.f) ? Health / MaxHealth : 0.f;
			HPBar->SetPercent(HPPercent);

			if (HPPercent < 0.3f)
			{
				HPBar->SetFillColorAndOpacity(FLinearColor::Red);
			}
		}

		if (UTextBlock* HPText = Cast<UTextBlock>(WidgetInstance->GetWidgetFromName(TEXT("HPText"))))
		{
			HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
		}
	}
}

void ASpartaCharacter::ApplySpeedDebuff(float Duration, float SpeedMultiplier)
{
	SpeedDebuffStack++;

	if (SpeedMultiplier < CurrentSpeedMultiplier)
	{
		CurrentSpeedMultiplier = SpeedMultiplier;
	}
	UpdateCharacterSpeed();

	FTimerHandle TempTimerHandle;
	GetWorldTimerManager().SetTimer(
		TempTimerHandle,
		this,
		&ASpartaCharacter::OnSpeedDebuffEnd,
		Duration,
		false
	);

	AddBuffInfoUI(TEXT("Speed Down"), Duration);
}

void ASpartaCharacter::OnSpeedDebuffEnd()
{
	SpeedDebuffStack = FMath::Max(0, SpeedDebuffStack - 1);
	if (SpeedDebuffStack == 0)
	{
		CurrentSpeedMultiplier = 1.0f;
		UpdateCharacterSpeed();
	}
}

void ASpartaCharacter::ApplyReverseControlsDebuff(float Duration)
{
	ReverseControlStack++;

	if (!bIsControlReversed)
	{
		bIsControlReversed = true;
	}

	FTimerHandle TempTimerHandle;
	GetWorldTimerManager().SetTimer(
		TempTimerHandle,
		this,
		&ASpartaCharacter::OnReverseControlsDebuffEnd,
		Duration,
		false
	);

	AddBuffInfoUI(TEXT("Reverse Control"), Duration);
}

void ASpartaCharacter::AddBuffInfoUI(const FString& BuffName, float Duration)
{
	if (ASpartaPlayerController* PC = Cast<ASpartaPlayerController>(GetController()))
	{
		UBuffInfo* ExistingBuff = nullptr;

		if (UUserWidget* HUDWidget = PC->GetHUDWidget())
		{
			UVerticalBox* BuffInfoStack = Cast<UVerticalBox>(HUDWidget->GetWidgetFromName(TEXT("BuffInfoStack")));
			
			if (!BuffInfoStack)
				return;

			for (UWidget* Child : BuffInfoStack->GetAllChildren())
			{
				UBuffInfo* BuffWidget = Cast<UBuffInfo>(Child);

				if (BuffWidget && BuffWidget->GetBuffName() == BuffName)
				{
					ExistingBuff = BuffWidget;
					break;
				}
			}

			if (ExistingBuff)
			{
				ExistingBuff->ExtendBuffDuration(Duration);
			}

			else
			{
				UBuffInfo* BuffInfoInstance = CreateWidget<UBuffInfo>(PC, BuffInfoWidgetClass);
				if (BuffInfoInstance)
				{
					BuffInfoStack->AddChild(BuffInfoInstance);
					BuffInfoInstance->InitializeBuffInfo(BuffName, Duration);
				}
			}
		}
	}
}

void ASpartaCharacter::OnReverseControlsDebuffEnd()
{
	ReverseControlStack = FMath::Max(0, ReverseControlStack - 1);
	if (ReverseControlStack == 0)
	{
		bIsControlReversed = false;
	}
}

void ASpartaCharacter::UpdateCharacterSpeed()
{
	const float BaseSpeed = bSprinting ? SprintSpeed : NormalSpeed;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * CurrentSpeedMultiplier;
}
