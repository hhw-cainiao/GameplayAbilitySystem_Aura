// HaiMa007


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

/*
 * 鼠标追踪 Actor，并高亮显示
 */
void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;	// 检查是否命中物体，如果没有命中，则直接返回

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());		// 命中的Actor是否能转换为接口类型

	/**
	 * Line trace from curson. There are several scenatios:
	 * A. LastActor is null && ThisActor is null
	 *		- Do nothing
	 * B. LastActor is null && ThisActor is valid
	 *		- Highlight ThisActor
	 * C. LastActor is valid && ThisActor is null
	 *		- UnHighlight LastActor
	 * D. Both actors are valid, but LastActor != ThisActor
	 *		- UnHightlight LastActor, and Hightlight ThisActor
	 * E. Both actors are valid, and are the same Actor
	 *		- Do nothing
	 */
	if (LastActor == nullptr && ThisActor)
	{
		// Case B
		ThisActor->HighlightActor();
	}

	if (LastActor)
	{
		if (ThisActor == nullptr)
		{
			// Case C
			LastActor->UnHighlightActor();
		}
		else
		{
			if (LastActor != ThisActor)
			{
				// Case D
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				// Case E - Do nothing
			}
		}
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

// 按下的回调
void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	// GameplayAbility 可以通过多种方式和 GameplayTag 相关联
	// 通过回调传入的 InputTag 参数，识别不同的 GameplayAbility，并激活

	// 鼠标左键的逻辑
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		// 当 ThisActor 有效时返回 true，无效时返回 false。
		bTargeting = ThisActor != nullptr;
		bAutoRunning = false;
	}
}

// 释放的回调
void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagReleased(InputTag);
}

// 保持的回调
void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	// 如果不是鼠标左键事件
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}
	
	// 是鼠标左键事件
	if (bTargeting)
	{
		// 鼠标瞄准了 目标Actor
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		// 鼠标没有瞄准 目标Actor，则进行AutoRunning
		FollowTime += GetWorld()->GetDeltaSeconds();

		FHitResult Hit;
		if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		{
			// 将目标位置设置为碰撞检测到的位置
			CachedDestination = Hit.Location;
		}

		if (APawn* ControllerPawn = GetPawn())
		{
			// 计算移动方向，然后添加位移
			const FVector WorldDirection = (CachedDestination - ControllerPawn->GetActorLocation()).GetSafeNormal();
			ControllerPawn->AddMovementInput(WorldDirection);
		}
	}
	
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	// 绑定输入事件到不同的回调函数（底层就是UE的委托，相当于绑定委托）
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	// BindAbilityActions 函数内部也是调用了 BindAction 函数进行绑定
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

/*
 * 移动操作的回调函数
 */
void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
	
}


