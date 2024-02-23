// HaiMa007


#include "UI/WidgetController/AuraWidgetController.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	this->PlayerController = WCParams.PlayerController;
	this->PlayerState = WCParams.PlayerState;
	this->AbilitySystemComponent = WCParams.AbilitySystemComponent;
	this->AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
	
}
