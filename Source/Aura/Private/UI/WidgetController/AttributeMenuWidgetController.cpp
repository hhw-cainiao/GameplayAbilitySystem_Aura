// HaiMa007


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

/*
 * 调用控制器对象的广播事件，用于初始化属性值（注意，此时我们不知道有哪些对象和这个 多播委托 绑定）
 */
void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);

	// 遍历 Tags 然后获取最新的 属性值，使用遍历而不是手动添加的方式，这样可以减少后序添加新属性时的工作量
	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	
}

/*
 * 当ASC中的属性值发生改变时的回调 GetGameplayAttributeValueChangeDelegate 
 */
void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	// GetGameplayAttributeValueChangeDelegate
	UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);
	
	for (auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}
	
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
