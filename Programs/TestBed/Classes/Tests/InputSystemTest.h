#pragma once

#include "Infrastructure/BaseScreen.h"
#include "Input/KeyboardInputDevice.h"
#include "Input/ActionSystem.h"
#include "Input/InputListener.h"

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
    void CreateKeyboardUI(DAVA::WideString header, DAVA::float32 x, DAVA::float32 y);
    void CreateMouseUI();
    void CreateTouchUI();
    void CreateActionsUI();
    void CreateInputListenerUI();

    void CreateKeyboardUIButton(DAVA::eInputElements key, DAVA::WideString text, DAVA::FTFont* font, DAVA::float32* x, DAVA::float32 y, DAVA::float32 w, DAVA::float32 h);

    void HighlightDigitalButton(DAVA::UIButton* button, DAVA::eDigitalElementStates state);

    bool OnInputEvent(DAVA::InputEvent const& event);
    void OnAction(DAVA::Action action);
    void OnInputListenerButtonPressed(DAVA::BaseObject* sender, void* data, void* callerData);
    void OnInputListeningEnded(DAVA::Vector<DAVA::InputEvent> input);
    void OnUpdate(DAVA::float32 delta);

    std::unordered_map<DAVA::uint32, DAVA::UIButton*> keyboardButtons;
    DAVA::Vector<DAVA::UIStaticText*> keyboardsHeaders;

    DAVA::UIButton* mouseBody;
    DAVA::UIStaticText* mouseHeader;
    std::unordered_map<DAVA::uint32, DAVA::UIButton*> mouseButtons;

    DAVA::UIStaticText* touchHeader;
    std::unordered_map<DAVA::uint32, DAVA::UIButton*> touchClickButtons;
    std::unordered_map<DAVA::uint32, DAVA::UIButton*> touchMoveButtons;

    std::unordered_map<DAVA::FastName, DAVA::UIStaticText*> actionCounters;

    DAVA::UIButton* inputListenerDigitalSingleWithoutModifiersButton;
    DAVA::UIButton* inputListenerDigitalSingleWithModifiersButton;
    DAVA::UIButton* inputListenerDigitalMultipleAnyButton;
    DAVA::UIButton* inputListenerAnalogButton;
    DAVA::UIStaticText* inputListenerResultField;

    DAVA::uint32 rawInputToken;
};
