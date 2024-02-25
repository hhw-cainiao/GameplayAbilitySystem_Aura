// HaiMa007

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include  "GameplayEffectTypes.h"
#include "AuraEffectActor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

/**
 * 增加 GE 的策略
 */
UENUM(BlueprintType)
enum class EEffectApplicationPolicy
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

/**
 * 移除 GE 的策略
 */
UENUM(BlueprintType)
enum class EEffectRemovalPolicy
{
	RemoveOnEndOverlap,
	DoNotRemove
};

USTRUCT(BlueprintType)
struct FInfiniteGE
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy= EEffectApplicationPolicy::DoNotApply;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

	
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraEffectActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	bool bDestroyOnEffectRemoval = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy= EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;
	
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;

	/**
	 * Side Quest：制作一个功能，一个Actor可以应用一系列的 GE
	 * 思路：
	 *	1. 针对 Instant GE：
	 *		- 创建一个 TMap<TSubclassOf<UGameplayEffect>, EEffectApplicationPolicy> InstantGEs 用于存储 Actor 所需要触发的所有 GE
	 *		- 随后在 OnOverlap 函数中一个个调用 ApplyEffectToTarget 函数即可
	 *	2. 针对 Duration GE：
	 *		- 同样创建 TMap<DurationGameplayEffectClass, DurationEffectApplicationPolicy> DurationGEs 用于存储 Actor 所需要触发的所有 GE
	 *		- 同样 OnOverlap 函数中一个个调用 ApplyEffectToTarget 函数即可
	 *	3. 针对 Infinite GE：
	 *		- 次情况和之前的不太一样，因为 Infinite GE 需要移除，移除时需要依赖于之前保存的 FActiveGameplayEffectHandle 类型的变量。
	 *		- 而且 Infinite GE 涉及三个需要存储的变量 InfiniteGameplayEffectClass, InfiniteEffectApplicationPolicy, InfiniteEffectRemovalPolicy
	 *		- 所以这里用 TMap 就无法存储了，这里创建一个结构体数据进行存储 
	 *		
	 */

	/**
	 * 不在之前的函数中直接修改，而是选择重新制作 OnOverlap 和 OnEndOverlap 函数用于测试，这样就不会影响之前代码的逻辑
	 */
	UFUNCTION(BlueprintCallable)
	void OnOverlapExpend(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlapExpend(AActor* TargetActor);
	
	/**
	 * 1. Instant GE
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	TMap<TSubclassOf<UGameplayEffect>, EEffectApplicationPolicy> InstantGEs;
	
	
	/**
	 * 2. Duration GE
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	TMap<TSubclassOf<UGameplayEffect>, EEffectApplicationPolicy> DurationGEs;
	
	
	/**
	 * 3. Infinite GE
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Applied Effects")
	TArray<FInfiniteGE> InfiniteGEs;
};
