// HaiMa007


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,
	bool bLogNotFound) const
{
	for (auto AbilityInputAction : AbilityInputActions)
	{
		if (AbilityInputAction.InputAction && AbilityInputAction.InputTag == InputTag)
		{
			return AbilityInputAction.InputAction;
		}
	}
	return nullptr;
}
