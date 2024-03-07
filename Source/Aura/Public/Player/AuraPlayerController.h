// HaiMa007

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UAuraAbilitySystemComponent;
class USplineComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;


	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();

	// 保存移动的目标位置
	FVector CachedDestination = FVector::ZeroVector;
	// 保存鼠标按下持续的时间，判断是不是短暂的点击
	float FollowTime = 0.f;
	// 快速点击的时间阈值，FollowTime小于这个值就是 “短按”
	float ShortPressThreshold = 0.5f;
	// 当 “短按” 时才会触发 AutoRunning 的逻辑，也就是自动寻路，如果是持续的按下，则不触发
	bool bAutoRunning = false;
	// 是否瞄准了目标Actor
	bool bTargeting = false;

	// 离目标距离多近时可以停止，这里使用一个圆形区域表示
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	// 保存自动移动的路线
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;
};
