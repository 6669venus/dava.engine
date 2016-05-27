#if defined(ENABLE_CEF_WEBVIEW)

#include <cef/include/cef_app.h>
#include "UI/Private/CEFDavaResourceHandler.h"

#include "Base/TypeHolders.h"
#include "Concurrency/Thread.h"
#include "Platform/SystemTimer.h"
#include "UI/Private/CEFController.h"

namespace DAVA
{
RefPtr<class CEFControllerImpl> cefControllerGlobal;

//--------------------------------------------------------------------------------------------------
// CEF application delegate
// Adds custom url scheme to scheme registrar
//--------------------------------------------------------------------------------------------------
class CEFDavaApp : public CefApp
{
    void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) override
    {
        registrar->AddCustomScheme("dava", false, false, false);
    }
    IMPLEMENT_REFCOUNTING(CEFDavaApp)
};

//--------------------------------------------------------------------------------------------------
// CEF controller private implementation
//--------------------------------------------------------------------------------------------------
class CEFControllerImpl : public RefCounter
{
public:
    CEFControllerImpl();
    ~CEFControllerImpl();

    void Update();
};

CEFControllerImpl::CEFControllerImpl()
{
    bool result = false;

    CefSettings settings;
    settings.no_sandbox = 1;
    settings.windowless_rendering_enabled = 1;
    // TODO: disable cef logs before merge
    //settings.log_severity = LOGSEVERITY_DISABLE;
    settings.log_severity = LOGSEVERITY_VERBOSE;
    CefString(&settings.browser_subprocess_path).FromASCII("CEFHelperProcess.exe");

    result = CefInitialize(CefMainArgs(), settings, new CEFDavaApp, nullptr);

    // CefInitialize replaces thread name, so we need to save it and restore
    // Restore name only on Main Thread
    if (result && Thread::IsMainThread())
    {
        // Restore name through InitMainThread
        Thread::InitMainThread();
    }

    // Register custom url scheme for dava-based applications
    result |= CefRegisterSchemeHandlerFactory("dava", "", new CEFDavaResourceHandlerFactory);
    DVASSERT_MSG(result == true, "CEF cannot be initialized");
}

CEFControllerImpl::~CEFControllerImpl()
{
    Update();
    CefShutdown();
}

void CEFControllerImpl::Update()
{
    CefDoMessageLoopWork();
}

//--------------------------------------------------------------------------------------------------
// CEF controller public implementation
//--------------------------------------------------------------------------------------------------
CEFController::CEFController()
{
    if (!cefControllerGlobal)
    {
        cefControllerGlobal.Set(new CEFControllerImpl);
    }
    cefControllerImpl = cefControllerGlobal;
}

CEFController::~CEFController() = default;

void CEFController::Update()
{
    cefControllerImpl->Update();
}

} // namespace DAVA

#endif // ENABLE_CEF_WEBVIEW
