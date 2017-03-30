#pragma once

#include "Infrastructure/BaseScreen.h"
#include "Input/KeyboardInputDevice.h"
#include "Input/ActionSystem.h"

#include <map>

class TestBed;
class InputSystemTest : public BaseScreen
{
public:
    InputSystemTest(TestBed& app);

protected:
    void LoadResources() override;
    void UnloadResources() override;

private:
    DAVA::UIButton* CreateKeyboardUIButton(DAVA::eInputElements key, DAVA::WideString text, DAVA::FTFont* font, DAVA::float32 x, DAVA::float32 y, DAVA::float32 w, DAVA::float32 h);
    bool OnInputEvent(DAVA::InputEvent const& event);
    void OnBeginFrame();
    void OnAction(DAVA::Action action);
    void OnInputListenerButtonPressed(DAVA::BaseObject* sender, void* data, void* callerData);
    void OnInputListeningEnded(DAVA::Vector<DAVA::eInputElements> input);

    void CreateKeyboardUI();
    void CreateMouseUI();
    void CreateActionsUI();
    void CreateInputListenerUI();

    std::unordered_map<DAVA::uint32, DAVA::UIButton*> keyboardButtons;
    std::unordered_map<DAVA::uint32, DAVA::UIButton*> mouseButtons;
    std::unordered_map<DAVA::FastName, DAVA::UIStaticText*> actionCounters;
    DAVA::UIStaticText* inputListenerResultField;

    DAVA::uint32 rawInputToken = 0;
};
