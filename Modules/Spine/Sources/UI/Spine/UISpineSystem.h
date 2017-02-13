#pragma once

#include "UI/UISystem.h"

namespace DAVA
{

class UISpineSystem : public UISystem
{
public:
    ~UISpineSystem() = default;

    void RegisterControl(UIControl* control) override;
    void UnregisterControl(UIControl* control) override;
    void RegisterComponent(UIControl* control, UIComponent* component) override;
    void UnregisterComponent(UIControl* control, UIComponent* component) override;

    void OnControlVisible(UIControl* control) override;
    void OnControlInvisible(UIControl* control) override;

    void Process(DAVA::float32 elapsedTime) override;
};

}