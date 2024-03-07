// HaiMa007


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// 已经初始化之后，就可以绑定委托
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
	
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (auto AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			GiveAbility(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	// 遍历所有的能力
	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		// 然后判断能力所包含的 Tag 是否和输入的 InputTag 相同，相同的话激活该能力
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			//  
			AbilitySpecInputPressed(AbilitySpec);
			// 当能力没有被激活时，才去激活技能，防止一直调用
			if (!AbilitySpec.IsActive())
			{
				// 激活技能
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	// 遍历所有的能力
	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		// 然后判断能力所包含的 Tag 是否和输入的 InputTag 相同
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec,
                                                FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	// 发出广播
	EffectAssetTags.Broadcast(TagContainer);
}
