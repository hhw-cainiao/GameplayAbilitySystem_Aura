// HaiMa007


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"

/*
 * 调用控制器对象的广播事件，用于初始化属性值（注意，此时我们不知道有哪些对象和这个 多播委托 绑定）
 */
void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet);

	check(AttributeInfo);
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Strength);
	Info.AttributeValue = AS->GetStrength();
	
	AttributeInfoDelegate.Broadcast(Info); 
}

/*
 * 当ASC中的属性值发生改变时的回调 GetGameplayAttributeValueChangeDelegate 
 */
void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	// GetGameplayAttributeValueChangeDelegate
	
	
}
