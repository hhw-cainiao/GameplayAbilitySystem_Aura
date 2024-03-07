// HaiMa007

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType> 
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};

/*
 * 让技能和按键在运行时可以动态绑定
 * 思路：目前还没太看懂，好像是将回调函数作为参数进行传递然后绑定函数
 * UAuraInputConfig 是一个 DataAsset，保存了 Tags 和 输入事件的结构体数组
 * PressedFuncType、ReleasedFuncType、HeldFuncType 是三个回调函数
 */
template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);

	for (const auto& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				// 将定义的输入事件，绑定到不同的回调函数，并且将 Action.InputTag参数传递给 PressedFunc 回调函数
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}
			if (ReleasedFunc)
			{
				// 将定义的输入事件，绑定到不同的回调函数，并且将 Action.InputTag参数传递给 ReleasedFunc 回调函数
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
			if (HeldFunc)
			{
				// 将定义的输入事件，绑定到不同的回调函数，并且将 Action.InputTag参数传递给 HeldFunc 回调函数
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}
	}
}
