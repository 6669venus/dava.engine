#include "Notification/Private/Win10/NativeDelegateWin10.h"

#if defined(__DAVAENGINE_WIN_UAP__)
#if defined(__DAVAENGINE_COREV2__)

#include "Engine/UWP/NativeServiceUWP.h"
#include "Notification/LocalNotificationController.h"

#include "Engine/Engine.h"
#include "Logger/Logger.h"
#include "Utils/UTF8Utils.h"
namespace DAVA
{
namespace Private
{
NativeDelegate::NativeDelegate(LocalNotificationController& controller)
    : localNotificationController(controller)
{
    Engine::Instance()->GetNativeService()->RegisterXamlApplicationListener(this);
}

NativeDelegate::~NativeDelegate()
{
    Engine::Instance()->GetNativeService()->UnregisterXamlApplicationListener(this);
}

void NativeDelegate::OnLaunched(::Windows::ApplicationModel::Activation::LaunchActivatedEventArgs ^ launchArgs)
{
    using namespace DAVA;
    String arguments = UTF8Utils::EncodeToUTF8(launchArgs->Arguments->Data());
    if (launchArgs->Kind == Windows::ApplicationModel::Activation::ActivationKind::Launch)
    {
        Platform::String ^ launchString = launchArgs->Arguments;
        if (!arguments.empty())
        {
            String uidStr = UTF8Utils::EncodeToUTF8(launchString->Data());
            auto function = [this, arguments]()
            {
                localNotificationController.OnNotificationPressed(arguments);
            };
            Engine::Instance()->RunAsyncOnMainThread(function);
        }
    }
}
} // namespace Private
} // namespace DAVA
#endif // __DAVAENGINE_COREV2__
#endif // __DAVAENGINE_WIN_UAP__
