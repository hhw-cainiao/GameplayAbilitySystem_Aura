// HaiMa007


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			FWidgetControllerParams WCParams;
			WCParams.PlayerState = PC->GetPlayerState<AAuraPlayerState>();
			WCParams.AttributeSet = Cast<AAuraPlayerState>(WCParams.PlayerState)->GetAttributeSet();
			WCParams.PlayerController = PC;
			WCParams.AbilitySystemComponent = Cast<AAuraPlayerState>(WCParams.PlayerState)->GetAbilitySystemComponent();
			return AuraHUD->GetOverlayWidgetController(WCParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			FWidgetControllerParams WCParams;
			WCParams.PlayerState = PC->GetPlayerState<AAuraPlayerState>();
			WCParams.AttributeSet = Cast<AAuraPlayerState>(WCParams.PlayerState)->GetAttributeSet();
			WCParams.PlayerController = PC;
			WCParams.AbilitySystemComponent = Cast<AAuraPlayerState>(WCParams.PlayerState)->GetAbilitySystemComponent();
			return AuraHUD->GetAttributeMenuWidgetController(WCParams);
		}
	}
	return nullptr;
}
