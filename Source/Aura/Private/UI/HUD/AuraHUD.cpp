// HaiMa007

#include "UI/HUD/AuraHUD.h"

#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	// 单例，全局只有一个 UOverlayWidgetController 类对象，如果已经存在则直接返回，如果不存在则根据结构体参数进行创建
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);

		// 调用绑定，将AbilitySystemComponent的委托和自身函数绑定起来。这样AttributeValue改变时，就可以通知到WidgetController
		OverlayWidgetController->BindCallbacksToDependencies();

		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}

/**
 * 这个函数在 AAuraCharacter 类的 InitAbilityActorInfo 函数处调用，因为在这个函数内部初始化了 InitOverlap 所需的参数信息
 * @param PC 
 * @param PS 
 * @param ASC 
 * @param AS 
 */
void AAuraHUD::InitOverlap(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_AuraHUD"));

	// 1. 在 HUD 中创建用户界面对象 OverlayWidget
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);

	// 2. 在 HUD 中创建用户界面控制对象 WidgetController
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	// 3. 调用用户界面对象 OverlayWidget 的 SetWidgetController 函数，将用户界面控制对象 WidgetController 和用户界面 OverlayWidget 进行关联
	OverlayWidget->SetWidgetController(WidgetController);

	// 4. 调用控制器对象的广播事件，用于初始化属性值（注意，此时我们不知道有哪些对象和这个 多播委托 绑定）
	WidgetController->BroadcastInitialValues();
	
	Widget->AddToViewport();

	
}

