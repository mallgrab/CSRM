#pragma once
#include <cstdint>

// DebugPanel::ExpeditionsPage: DebugPanel::Panel, DebugPanel::Page, app::EventHandler
struct DebugPanel
{
	// TODO: majority of the args are not filled in, mouse event & drawmenu are correct
	virtual bool Dtor() = 0;
	virtual bool ConsoleDispatch() = 0;
	virtual bool OnUserRenderSettingChangedEvent() = 0;
	virtual bool DeviceChangedEvent() = 0;
	virtual bool AltEnterEvent() = 0;
	virtual bool WindowResizeEvent() = 0;
	virtual bool ExpansionEvent() = 0;
	virtual bool CheckBoxControlEvent() = 0;
	virtual bool tmp9() = 0;
	virtual bool tmp10() = 0;
	virtual bool InputEvent() = 0;
	virtual bool GamepadThumbEvent() = 0;
	virtual bool GamepadTriggerEvent() = 0;
	virtual bool GamepadButtonEvent() = 0;
	virtual bool MouseScrollEvent(void* MouseScrollEvent) = 0; // app::MouseScrollEvent *a2
	virtual bool MouseMotionEvent(void* MouseMotionEvent) = 0;
	virtual bool MouseButtonEvent(void* MouseButtonEvent) = 0;
	virtual bool RawKeyEvent() = 0;
	virtual bool KeyEvent() = 0;
	virtual bool SpecialKeyEvent() = 0;
	virtual bool CharEvent() = 0;
	virtual bool MaximizeEvent() = 0;
	virtual bool MinimizeEvent() = 0;
	virtual bool FocusEvent() = 0;
	virtual bool CloseEvent() = 0;
	virtual bool ResetPanel() = 0;
	virtual bool tmp27() = 0;
	virtual bool DrawMenu() = 0;

	char* name = nullptr;
};