// HaiMa007


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"


AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}


void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	// 通过统一的 蓝图函数库 获取 ASC 比自己通过 AbilitySystemInterface 然后获取要更加规范。
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;

	check(GameplayEffectClass);
	
	// 通过 ASC 和 UGameplayEffect 类，对 Target 应用 Effect
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite == true && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}
	
	
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		// 还需要找到对应的 ASC和 FActiveGameplayEffectHandle 句柄，两者一定是对应的。
		for (auto HandlePair : ActiveEffectHandles)
		{
			// ActiveEffectHandles 的键值对是 FActiveGameplayEffectHandle : ASC
			if (TargetASC == HandlePair.Value)
			{
				// RemoveActiveGameplayEffect函数还可以控制移除的 Stacks 的数量，默认是移除全部，我们这里只移除一个 Stack 
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				// 移除 GE 后，这对存储的键值对数据也不需要了，移除即可。记住不要在循环里面删除，不然会导致程序崩溃，在循环体外进行元素移除
				HandlesToRemove.Add(HandlePair.Key);
			}
		}
		// 移除 GE 后，这对存储的键值对数据也不需要了，移除即可。记住不要在循环里面删除，不然会导致程序崩溃，在循环体外进行元素移除
		for (auto& Handle :HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}

void AAuraEffectActor::OnOverlapExpend(AActor* TargetActor)
{
	// 遍历 InstantGEs 容器，然后依次 Apply GE
	for (const auto InstantGE : InstantGEs)
	{
		// 判断 EEffectApplicationPolicy 类型
		if (InstantGE.Value == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			// 应用 GE
			ApplyEffectToTarget(TargetActor, InstantGE.Key);
		}
	}
	// 遍历 DurationGEs 容器，然后依次 Apply GE
	for (const auto DurationGE : DurationGEs)
	{
		// 判断 EEffectApplicationPolicy 类型
		if (DurationGE.Value == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			// 应用 GE
			ApplyEffectToTarget(TargetActor, DurationGE.Key);
		}
	}
	// 遍历 InfiniteGEs 容器，然后依次 Apply GE
	for (const auto InfiniteGE : InfiniteGEs)
	{
		if (InfiniteGE.InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			// 应用 GE
			ApplyEffectToTarget(TargetActor, InfiniteGE.InfiniteGameplayEffectClass);
		}
	}
}

void AAuraEffectActor::OnEndOverlapExpend(AActor* TargetActor)
{
	// 遍历 InstantGEs 容器，然后依次 Apply GE
	for (const auto InstantGE : InstantGEs)
	{
		// 判断 EEffectApplicationPolicy 类型
		if (InstantGE.Value == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			// 应用 GE
			ApplyEffectToTarget(TargetActor, InstantGE.Key);
		}
	}
	// 遍历 DurationGEs 容器，然后依次 Apply GE
	for (const auto DurationGE : DurationGEs)
	{
		// 判断 EEffectApplicationPolicy 类型
		if (DurationGE.Value == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			// 应用 GE
			ApplyEffectToTarget(TargetActor, DurationGE.Key);
		}
	}
	// 遍历 InfiniteGEs 容器，然后依次 Apply GE
	for (const auto InfiniteGE : InfiniteGEs)
	{
		if (InfiniteGE.InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			// 应用 GE
			ApplyEffectToTarget(TargetActor, InfiniteGE.InfiniteGameplayEffectClass);
		}
	}

	/**
	 * 移除 InfiniteGEs 的逻辑：
	 *		存储 InfiniteGEs 的 FActiveGameplayEffectHandle 对象不能存储在 TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> 对象中
	 *		因为这些 InfiniteGEs 都是存储在同一个 Actor 上，同一个 Actor 变量获取的 UAbilitySystemComponent 肯定是相同的，所以无法作为不同 GE 的唯一标识。
	 *		
	 * 
	 */
	// 遍历 InfiniteGEs 容器，然后依次移除 GE
	for (const auto InfiniteGE : InfiniteGEs)
	{
		if (InfiniteGE.InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
		{
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
			if (!IsValid(TargetASC)) return;

			TArray<FActiveGameplayEffectHandle> HandlesToRemove;
			// 还需要找到对应的 ASC和 FActiveGameplayEffectHandle 句柄，两者一定是对应的。
			for (auto HandlePair : ActiveEffectHandles)
			{
				// ActiveEffectHandles 的键值对是 FActiveGameplayEffectHandle : ASC
				if (TargetASC == HandlePair.Value)
				{
					// RemoveActiveGameplayEffect函数还可以控制移除的 Stacks 的数量，默认是移除全部，我们这里只移除一个 Stack 
					TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
					// 移除 GE 后，这对存储的键值对数据也不需要了，移除即可。记住不要在循环里面删除，不然会导致程序崩溃，在循环体外进行元素移除
					HandlesToRemove.Add(HandlePair.Key);
				}
			}
			// 移除 GE 后，这对存储的键值对数据也不需要了，移除即可。记住不要在循环里面删除，不然会导致程序崩溃，在循环体外进行元素移除
			for (auto& Handle :HandlesToRemove)
			{
				ActiveEffectHandles.FindAndRemoveChecked(Handle);
			}
		}
	}// for (const auto InfiniteGE : InfiniteGEs)

	
}

