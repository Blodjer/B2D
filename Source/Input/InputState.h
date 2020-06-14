#pragma once

class InputState
{
public:
    InputState();
    InputState(InputState& inputState);
    ~InputState() = default;

    bool IsKey(EKey key, EKeyEvent keyEvent);

    EKeyEvent const& GetKeyState(EKey key);
    void SetKeyState(EKey key, EKeyEvent event);

    void Flush();

private:
    std::array<EKeyEvent, g_numberOfKeys> m_keyMap;
};
