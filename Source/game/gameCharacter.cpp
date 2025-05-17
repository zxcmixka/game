#include "gameCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

gameCharacter::gameCharacter()
{
    // Настройка коллайдера
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

    // Создание компонента камеры
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
    FirstPersonCameraComponent->bUsePawnControlRotation = true;

    // Создание меша персонажа
    Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
    Mesh1P->SetOnlyOwnerSee(true);
    Mesh1P->SetupAttachment(FirstPersonCameraComponent);
    Mesh1P->bCastDynamicShadow = false;
    Mesh1P->CastShadow = false;
    Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

    // Настройка движения
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = WalkSpeed;
        MoveComp->bOrientRotationToMovement = true;
        MoveComp->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    }

    // Инициализация выносливости
    CurrentStamina = MaxStamina;
    bIsSprinting = false;
}

void gameCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Добавление маппинга ввода
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void gameCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Обновление выносливости
    if (bIsSprinting)
    {
        UpdateStamina(DeltaTime);
    }
    else if (CurrentStamina < MaxStamina)
    {
        CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + StaminaRecoveryRate * DeltaTime);
    }
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Прыжок
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        // Движение
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);

        // Вращение камеры
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);

        // Спринт
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMyCharacter::StartSprint);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMyCharacter::StopSprint);
    }
}

void gameCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddMovementInput(GetActorForwardVector(), MovementVector.Y);
        AddMovementInput(GetActorRightVector(), MovementVector.X);
    }
}

void gameCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void gameCharacter::StartSprint()
{
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        const FVector Velocity = MoveComp->Velocity;
        const float ForwardSpeed = FVector::DotProduct(GetActorForwardVector(), Velocity.GetSafeNormal());

        if (CurrentStamina > 10.0f && ForwardSpeed > 0.1f)
        {
            bIsSprinting = true;
            MoveComp->MaxWalkSpeed = SprintSpeed;
        }
    }
}

void gameCharacter::StopSprint()
{
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        bIsSprinting = false;
        MoveComp->MaxWalkSpeed = WalkSpeed;
    }
}

void gameCharacter::UpdateStamina(float DeltaTime)
{
    CurrentStamina = FMath::Max(0.0f, CurrentStamina - StaminaDrainRate * DeltaTime);

    if (CurrentStamina <= 0.0f)
    {
        StopSprint();
    }
}