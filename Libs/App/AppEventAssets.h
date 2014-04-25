
#include <Windows.h>
#include <Windowsx.h>

#ifndef __NENA_APPEVENTASSETS_INCLUDED__
#define __NENA_APPEVENTASSETS_INCLUDED__

namespace Nena
{
	namespace Application
	{
		struct VirtualKey2
		{
			static const UINT32 None = 0; // No virtual key value.

			// Letters

			static const UINT32 A = L'A'; // The letter "A" key. 
			static const UINT32 B = L'B'; // The letter "B" key. 
			static const UINT32 C = L'C'; // The letter "C" key. 
			static const UINT32 D = L'D'; // The letter "D" key.
			static const UINT32 E = L'E'; // The letter "E" key. 
			static const UINT32 F = L'F'; // The letter "F" key. 
			static const UINT32 G = L'G'; // The letter "G" key.  
			static const UINT32 H = L'H'; // The letter "H" key. 
			static const UINT32 I = L'I'; // The letter "I" key. 
			static const UINT32 J = L'J'; // The letter "J" key. 
			static const UINT32 K = L'K'; // The letter "K" key. 
			static const UINT32 L = L'L'; // The letter "L" key. 
			static const UINT32 M = L'M'; // The letter "M" key. 
			static const UINT32 N = L'N'; // The letter "N" key. 
			static const UINT32 O = L'O'; // The letter "O" key. 
			static const UINT32 P = L'P'; // The letter "P" key. 
			static const UINT32 Q = L'Q'; // The letter "Q" key. 
			static const UINT32 R = L'R'; // The letter "R" key. 
			static const UINT32 S = L'S'; // The letter "S" key. 
			static const UINT32 T = L'T'; // The letter "T" key. 
			static const UINT32 U = L'U'; // The letter "U" key. 
			static const UINT32 V = L'V'; // The letter "V" key. 
			static const UINT32 W = L'W'; // The letter "W" key. 
			static const UINT32 X = L'X'; // The letter "X" key. 
			static const UINT32 Y = L'Y'; // The letter "Y" key. 
			static const UINT32 Z = L'Z'; // The letter "Z" key. 

			// Numbers

			static const UINT32 NumericKeyLock = VK_NUMLOCK; // The Num Lock key.
			static const UINT32 Number0 = L'0';	 // The number "0" key. 
			static const UINT32 Number1 = Number0 + 1; // The number "1" key. 
			static const UINT32 Number2 = Number0 + 2; // The number "2" key. 
			static const UINT32 Number3 = Number0 + 3; // The number "3" key. 
			static const UINT32 Number4 = Number0 + 4; // The number "4" key. 
			static const UINT32 Number5 = Number0 + 5; // The number "5" key.  
			static const UINT32 Number6 = Number0 + 6; // The number "6" key. 
			static const UINT32 Number7 = Number0 + 7; // The number "7" key. 
			static const UINT32 Number8 = Number0 + 8; // The number "8" key. 
			static const UINT32 Number9 = Number0 + 9; // The number "9" key. 
			static const UINT32 NumberPad0 = VK_NUMPAD0; // The number "0" key as located on a numeric pad.
			static const UINT32 NumberPad1 = NumberPad0 + 1; // The number "1" key as located on a numeric pad.
			static const UINT32 NumberPad2 = NumberPad0 + 2; // The number "2" key as located on a numeric pad.
			static const UINT32 NumberPad3 = NumberPad0 + 3; // The number "3" key as located on a numeric pad.
			static const UINT32 NumberPad4 = NumberPad0 + 4; // The number "4" key as located on a numeric pad.
			static const UINT32 NumberPad5 = NumberPad0 + 5; // The number "5" key as located on a numeric pad.
			static const UINT32 NumberPad6 = NumberPad0 + 6; // The number "6" key as located on a numeric pad.
			static const UINT32 NumberPad7 = NumberPad0 + 7; // The number "7" key as located on a numeric pad.
			static const UINT32 NumberPad8 = NumberPad0 + 8; // The number "8" key as located on a numeric pad.
			static const UINT32 NumberPad9 = NumberPad0 + 9; // The number "9" key as located on a numeric pad.

			// Math 

			static const UINT32 Add = VK_ADD; // The add (+) operation key as located on a numeric pad.
			static const UINT32 Divide = VK_DIVIDE; // The divide (/) operation key as located on a numeric pad.
			static const UINT32 Multiply = VK_MULTIPLY; // The multiply (*) operation key as located on a numeric pad.
			static const UINT32 Subtract = VK_SUBTRACT; // The subtract (-) operation key as located on a numeric pad.

			// Function Keys

			static const UINT32 F1 = VK_F1;	// The function key 1.
			static const UINT32 F2 = VK_F2;	// The function key 2. 
			static const UINT32 F3 = VK_F3;	// The function key 3.
			static const UINT32 F4 = VK_F4;	// The function key 4.
			static const UINT32 F5 = VK_F5;	// The function key 5.
			static const UINT32 F6 = VK_F6;	// The function key 6.
			static const UINT32 F7 = VK_F7;	// The function key 7.
			static const UINT32 F8 = VK_F8;	// The function key 8.
			static const UINT32 F9 = VK_F9;	// The function key 9.
			static const UINT32 F10 = VK_F10; // The function key 10.
			static const UINT32 F11 = VK_F11; // The function key 11.
			static const UINT32 F12 = VK_F12; // The function key 12.
			static const UINT32 F13 = VK_F13; // The function key 13.
			static const UINT32 F14 = VK_F14; // The function key 14.
			static const UINT32 F15 = VK_F15; // The function key 15.
			static const UINT32 F16 = VK_F16; // The function key 16. 
			static const UINT32 F17 = VK_F17; // The function key 17.
			static const UINT32 F18 = VK_F18; // The function key 18.
			static const UINT32 F19 = VK_F19; // The function key 19.
			static const UINT32 F20 = VK_F20; // The function key 20.
			static const UINT32 F21 = VK_F21; // The function key 21.
			static const UINT32 F22 = VK_F22; // The function key 22.
			static const UINT32 F23 = VK_F23; // The function key 23.
			static const UINT32 F24 = VK_F24; // The function key 24. 

			// Special

			static const UINT32 Accept = VK_ACCEPT; // The accept button or key.
			static const UINT32 Application = VK_APPS; // The application key or button.
			static const UINT32 Back = VK_BACK;	 // The virtual back key or button.
			static const UINT32 Cancel = VK_CANCEL; // The cancel key or button.
			static const UINT32 CapitalLock = VK_CAPITAL; // The Caps Lock key or button.
			static const UINT32 Clear = VK_CLEAR; // The Clear key or button.
			static const UINT32 Convert = VK_CONVERT; // The convert button or key.
			static const UINT32 NonConvert = VK_NONCONVERT; // The nonconvert button or key.
			static const UINT32 Decimal = VK_DECIMAL; // The Ctrl key. 
			static const UINT32 Delete = VK_DELETE; // The Delete key.
			static const UINT32 End = VK_END; // The End key.
			static const UINT32 Enter = VK_RETURN; // The Enter key.
			static const UINT32 Escape = VK_ESCAPE; // The Esc key.
			static const UINT32 Execute = VK_EXECUTE; // The execute key or button.
			static const UINT32 Final = VK_FINAL; // The Final symbol key-shift button.
			static const UINT32 Help = VK_HELP; // The Help key or button.
			static const UINT32 Home = VK_HOME; // The Home key.
			static const UINT32 Insert = VK_INSERT; // The Insert key.
			static const UINT32 ModeChange = VK_MODECHANGE; // The mode change key.
			static const UINT32 Pause = VK_PAUSE; // The Pause key or button. 
			static const UINT32 Print = VK_PRINT; // The Print key or button. 
			static const UINT32 Scroll = VK_SCROLL; // The Scroll Lock (ScrLk) key.
			static const UINT32 Select = VK_SELECT; // The Select key or button.
			static const UINT32 Separator = VK_SEPARATOR; // The separator key as located on a numeric pad.
			static const UINT32 Sleep = VK_SLEEP; // The sleep key or button.
			static const UINT32 Snapshot = VK_SNAPSHOT; // The snapshot key or button.
			static const UINT32 Space = VK_SPACE; // The Spacebar key or button.
			static const UINT32 Tab = VK_TAB; // The Tab key.

			// Pairs 

			static const UINT32 LeftButton = VK_LBUTTON; // The left mouse button.
			static const UINT32 RightButton = VK_RBUTTON; // The right mouse button.
			static const UINT32 MiddleButton = VK_MBUTTON; // The middle mouse button.
			static const UINT32 Control = VK_CONTROL; // The Ctrl key. 
			static const UINT32 LeftControl = VK_LCONTROL; // The left Ctrl key.
			static const UINT32 RightControl = VK_RCONTROL; // The right Ctrl key.
			static const UINT32 Menu = VK_MENU; // The menu key or button.
			static const UINT32 LeftMenu = VK_LMENU; // The left menu key.
			static const UINT32 RightMenu = VK_RMENU; // The right menu key.
			static const UINT32 Shift = VK_SHIFT; // The Shift key.
			static const UINT32 LeftShift = VK_LSHIFT; // The left Shift key.
			static const UINT32 RightShift = VK_RSHIFT; // The right Shift key.
			static const UINT32 LeftWindows = VK_LWIN; // The left Windows key.
			static const UINT32 RightWindows = VK_RWIN; // The right Windows key.
			static const UINT32 PageDown = VK_NEXT; // The Page Down key.
			static const UINT32 PageUp = VK_PRIOR; // The Page Up key.

			// Arrows

			static const UINT32 Left = VK_LEFT; // The Left Arrow key.
			static const UINT32 Up = VK_UP; // The Up Arrow key.
			static const UINT32 Down = VK_DOWN; // The Down Arrow key.
			static const UINT32 Right = VK_RIGHT; // The Right Arrow key.

			// Extra

			static const UINT32 XButton1 = VK_XBUTTON1; // An additional "extended" device key or button (for example; an additional mouse button).
			static const UINT32 XButton2 = VK_XBUTTON2; // An additional "extended" device key or button (for example; an additional mouse button).

			// Wtf

			static const UINT32 Hangul = VK_HANGUL; // The Hangul symbol key-shift button.
			static const UINT32 Hanja = VK_HANJA; // The Hanja symbol key shift button.
			static const UINT32 Junja = VK_JUNJA; // The Junja symbol key-shift button.
			static const UINT32 Kana = VK_KANA; // The Kana symbol key-shift button.
			static const UINT32 Kanji = VK_KANJI; // The Kanji symbol key-shift button.

		};

		enum class VirtualKey1 : UINT32
		{
			None = 0, // No virtual key value.

			// Letters

			A = L'A', // The letter "A" key. 
			B = L'B', // The letter "B" key. 
			C = L'C', // The letter "C" key. 
			D = L'D', // The letter "D" key.
			E = L'E', // The letter "E" key. 
			F = L'F', // The letter "F" key. 
			G = L'G', // The letter "G" key.  
			H = L'H', // The letter "H" key. 
			I = L'I', // The letter "I" key. 
			J = L'J', // The letter "J" key. 
			K = L'K', // The letter "K" key. 
			L = L'L', // The letter "L" key. 
			M = L'M', // The letter "M" key. 
			N = L'N', // The letter "N" key. 
			O = L'O', // The letter "O" key. 
			P = L'P', // The letter "P" key. 
			Q = L'Q', // The letter "Q" key. 
			R = L'R', // The letter "R" key. 
			S = L'S', // The letter "S" key. 
			T = L'T', // The letter "T" key. 
			U = L'U', // The letter "U" key. 
			V = L'V', // The letter "V" key. 
			W = L'W', // The letter "W" key. 
			X = L'X', // The letter "X" key. 
			Y = L'Y', // The letter "Y" key. 
			Z = L'Z', // The letter "Z" key. 

			// Numbers

			NumericKeyLock = VK_NUMLOCK, // The Num Lock key.
			Number0 = L'0',	 // The number "0" key. 
			Number1 = Number0 + 1, // The number "1" key. 
			Number2 = Number0 + 2, // The number "2" key. 
			Number3 = Number0 + 3, // The number "3" key. 
			Number4 = Number0 + 4, // The number "4" key. 
			Number5 = Number0 + 5, // The number "5" key.  
			Number6 = Number0 + 6, // The number "6" key. 
			Number7 = Number0 + 7, // The number "7" key. 
			Number8 = Number0 + 8, // The number "8" key. 
			Number9 = Number0 + 9, // The number "9" key. 
			NumberPad0 = VK_NUMPAD0, // The number "0" key as located on a numeric pad.
			NumberPad1 = NumberPad0 + 1, // The number "1" key as located on a numeric pad.
			NumberPad2 = NumberPad0 + 2, // The number "2" key as located on a numeric pad.
			NumberPad3 = NumberPad0 + 3, // The number "3" key as located on a numeric pad.
			NumberPad4 = NumberPad0 + 4, // The number "4" key as located on a numeric pad.
			NumberPad5 = NumberPad0 + 5, // The number "5" key as located on a numeric pad.
			NumberPad6 = NumberPad0 + 6, // The number "6" key as located on a numeric pad.
			NumberPad7 = NumberPad0 + 7, // The number "7" key as located on a numeric pad.
			NumberPad8 = NumberPad0 + 8, // The number "8" key as located on a numeric pad.
			NumberPad9 = NumberPad0 + 9, // The number "9" key as located on a numeric pad.

			// Math 

			Add = VK_ADD, // The add (+) operation key as located on a numeric pad.
			Divide = VK_DIVIDE, // The divide (/) operation key as located on a numeric pad.
			Multiply = VK_MULTIPLY, // The multiply (*) operation key as located on a numeric pad.
			Subtract = VK_SUBTRACT, // The subtract (-) operation key as located on a numeric pad.

			// Function Keys

			F1 = VK_F1,	// The function key 1.
			F2 = VK_F2,	// The function key 2. 
			F3 = VK_F3,	// The function key 3.
			F4 = VK_F4,	// The function key 4.
			F5 = VK_F5,	// The function key 5.
			F6 = VK_F6,	// The function key 6.
			F7 = VK_F7,	// The function key 7.
			F8 = VK_F8,	// The function key 8.
			F9 = VK_F9,	// The function key 9.
			F10 = VK_F10, // The function key 10.
			F11 = VK_F11, // The function key 11.
			F12 = VK_F12, // The function key 12.
			F13 = VK_F13, // The function key 13.
			F14 = VK_F14, // The function key 14.
			F15 = VK_F15, // The function key 15.
			F16 = VK_F16, // The function key 16. 
			F17 = VK_F17, // The function key 17.
			F18 = VK_F18, // The function key 18.
			F19 = VK_F19, // The function key 19.
			F20 = VK_F20, // The function key 20.
			F21 = VK_F21, // The function key 21.
			F22 = VK_F22, // The function key 22.
			F23 = VK_F23, // The function key 23.
			F24 = VK_F24, // The function key 24. 

			// Special

			Accept = VK_ACCEPT, // The accept button or key.
			Application = VK_APPS, // The application key or button.
			Back = VK_BACK,	 // The virtual back key or button.
			Cancel = VK_CANCEL, // The cancel key or button.
			CapitalLock = VK_CAPITAL, // The Caps Lock key or button.
			Clear = VK_CLEAR, // The Clear key or button.
			Convert = VK_CONVERT, // The convert button or key.
			NonConvert = VK_NONCONVERT, // The nonconvert button or key.
			Decimal = VK_DECIMAL, // The Ctrl key. 
			Delete = VK_DELETE, // The Delete key.
			End = VK_END, // The End key.
			Enter = VK_RETURN, // The Enter key.
			Escape = VK_ESCAPE, // The Esc key.
			Execute = VK_EXECUTE, // The execute key or button.
			Final = VK_FINAL, // The Final symbol key-shift button.
			Help = VK_HELP, // The Help key or button.
			Home = VK_HOME, // The Home key.
			Insert = VK_INSERT, // The Insert key.
			ModeChange = VK_MODECHANGE, // The mode change key.
			Pause = VK_PAUSE, // The Pause key or button. 
			Print = VK_PRINT, // The Print key or button. 
			Scroll = VK_SCROLL, // The Scroll Lock (ScrLk) key.
			Select = VK_SELECT, // The Select key or button.
			Separator = VK_SEPARATOR, // The separator key as located on a numeric pad.
			Sleep = VK_SLEEP, // The sleep key or button.
			Snapshot = VK_SNAPSHOT, // The snapshot key or button.
			Space = VK_SPACE, // The Spacebar key or button.
			Tab = VK_TAB, // The Tab key.

			// Pairs 

			LeftButton = VK_LBUTTON, // The left mouse button.
			RightButton = VK_RBUTTON, // The right mouse button.
			MiddleButton = VK_MBUTTON, // The middle mouse button.
			Control = VK_CONTROL, // The Ctrl key. 
			LeftControl = VK_LCONTROL, // The left Ctrl key.
			RightControl = VK_RCONTROL, // The right Ctrl key.
			Menu = VK_MENU, // The menu key or button.
			LeftMenu = VK_LMENU, // The left menu key.
			RightMenu = VK_RMENU, // The right menu key.
			Shift = VK_SHIFT, // The Shift key.
			LeftShift = VK_LSHIFT, // The left Shift key.
			RightShift = VK_RSHIFT, // The right Shift key.
			LeftWindows = VK_LWIN, // The left Windows key.
			RightWindows = VK_RWIN, // The right Windows key.
			PageDown = VK_NEXT, // The Page Down key.
			PageUp = VK_PRIOR, // The Page Up key.

			// Arrows

			Left = VK_LEFT, // The Left Arrow key.
			Up = VK_UP, // The Up Arrow key.
			Down = VK_DOWN, // The Down Arrow key.
			Right = VK_RIGHT, // The Right Arrow key.

			// Extra

			XButton1 = VK_XBUTTON1, // An additional "extended" device key or button (for example, an additional mouse button).
			XButton2 = VK_XBUTTON2, // An additional "extended" device key or button (for example, an additional mouse button).

			// Wtf

			Hangul = VK_HANGUL, // The Hangul symbol key-shift button.
			Hanja = VK_HANJA, // The Hanja symbol key shift button.
			Junja = VK_JUNJA, // The Junja symbol key-shift button.
			Kana = VK_KANA, // The Kana symbol key-shift button.
			Kanji = VK_KANJI, // The Kanji symbol key-shift button.

		};

		struct VirtualKeyModifiers2
		{
			static const UINT32 None = 0 << 0; // No virtual key modifier.
			static const UINT32 Control = 1 << 0; // The Ctrl (control) virtual key.
			static const UINT32 Shift = 1 << 2; // The Shift virtual key.
			static const UINT32 Menu = 1 << 1; // The Menu virtual key.
			static const UINT32 Windows = 1 << 3; // The Windows virtual key.
		};

		enum class VirtualKeyModifiers1 : UINT32
		{
			None = 0 << 0, // No virtual key modifier.
			Control = 1 << 0, // The Ctrl (control) virtual key.
			Shift = 1 << 2, // The Shift virtual key.
			Menu = 1 << 1, // The Menu virtual key.
			Windows = 1 << 3, // The Windows virtual key.
		};

		struct Message2
		{
			static const UINT32 Other = 0;
			static const UINT32 MouseMove = WM_MOUSEMOVE;
			static const UINT32 LeftButtonPressed = WM_LBUTTONDOWN;
			static const UINT32 LeftButtonReleased = WM_LBUTTONUP;
			static const UINT32 RightButtonPressed = WM_RBUTTONDOWN;
			static const UINT32 RightButtonReleased = WM_RBUTTONUP;
			static const UINT32 MiddleButtonPressed = WM_MBUTTONDOWN;
			static const UINT32 MiddleButtonReleased = WM_MBUTTONUP;
			static const UINT32 MouseWheel = WM_MOUSEWHEEL;
			static const UINT32 MouseHWheel = WM_MOUSEHWHEEL;
			static const UINT32 KeyPressed = WM_KEYDOWN;
			static const UINT32 KeyReleased = WM_KEYUP;
			static const UINT32 Destroy = WM_DESTROY;
			static const UINT32 Close = WM_CLOSE;
			static const UINT32 Quit = WM_QUIT;
			static const UINT32 ExitSizeMove = WM_EXITSIZEMOVE;
			static const UINT32 Paint = WM_PAINT;
			static const UINT32 Timer = WM_TIMER;
		};

		typedef enum Message1 : UINT32
		{
			Other = 0,
			MouseMove = WM_MOUSEMOVE,
			LeftButtonPressed = WM_LBUTTONDOWN,
			LeftButtonReleased = WM_LBUTTONUP,
			RightButtonPressed = WM_RBUTTONDOWN,
			RightButtonReleased = WM_RBUTTONUP,
			MiddleButtonPressed = WM_MBUTTONDOWN,
			MiddleButtonReleased = WM_MBUTTONUP,
			MouseWheel = WM_MOUSEWHEEL,
			MouseHWheel = WM_MOUSEHWHEEL,
			KeyPressed = WM_KEYDOWN,
			KeyReleased = WM_KEYUP,
			Destroy = WM_DESTROY,
			Close = WM_CLOSE,
			Quit = WM_QUIT,
		};

		struct CorePhysicalKeyStatus
		{
			BOOL WasKeyDown = FALSE; // Whether a key is currently pressed down.
			BOOL IsKeyReleased = TRUE; // Whether a key has moved from a pressed to a released status.
			BOOL IsMenuKeyDown = FALSE; // Whether the menu key is currently pressed down.
			BOOL IsExtendedKey = FALSE; // Whether the key that was pressed maps to an extended ASCII character.
			UINT32 RepeatCount = 1u; // The number of times a key was pressed.
			UINT32 ScanCode = 0u; // The scan code for a key that was pressed.

			static inline BOOL Extract_WasKeyDown(_In_ LPARAM v) { return v & 0x02000000; } // bits=[29-29]
			static inline BOOL Extract_IsKeyReleased(_In_ LPARAM v) { return v & 0x04000000; } // bits=[30-30]
			static inline BOOL Extract_IsMenuKeyDown(_In_ LPARAM v) { return v & 0x08000000; } // bits=[31-31]
			static inline BOOL Extract_IsExtendedKey(_In_ LPARAM v) { return v & 0x00100000; } // bits=[24-24] + reserved=[25-28]
			static inline UINT32 Extract_RepeatCount(_In_ LPARAM v) { return v & 0x0000ffff; } // bits=[00-15]
			static inline UINT32 Extract_ScanCode(_In_ LPARAM v) { return v & 0x00ff0000; } // bits=[16-23]

			static inline void ExtractSlim(_In_ LPARAM param, _Out_ UINT32 &prevState, _Out_ UINT32 &count)
			{
				prevState = Extract_IsKeyReleased(param);
				count = Extract_RepeatCount(param);
			}
		};

		enum class CoreCursorType : UINT32
		{
			Arrow, // The left-upward (northwest) arrow Windows cursor.
			Cross, // The "cross" Windows cursor.
			Custom,	 // A custom cursor.
			Hand, // The "hand" Windows cursor.
			Help, // The "question mark" Windows cursor.
			IBeam, // The "I"-shaped Windows cursor used for text selection.
			SizeAll, // The "cross arrow" Windows cursor used for user interface (UI) element sizing.
			SizeNortheastSouthwest, // The "right-upward, left-downward" dual arrow Windows cursor often used for element sizing.
			SizeNorthSouth, // The up-down dual arrow Windows cursor often used for vertical (height) sizing.
			SizeNorthwestSoutheast, // The "left-upward, right-downward" dual arrow Windows cursor often used for element sizing.
			SizeWestEast, // The left-right dual arrow Windows cursor often used for horizontal (width) sizing.
			UniversalNo, // The red "circle slash" Windows cursor often used to indicate that a UI behavor cannot be performed.
			UpArrow, // The up arrow Windows cursor.
			Wait, // The cycling Windows "wait" cursor often used to indicate that an element or behavior is in a wait state and cannot respond at the time.

		};

		enum class CoreVirtualKeyStates : UINT32
		{
			None, // The key is up or in no specific state.
			Down, // The key is pressed down for the input event.
			Locked, // The key is in a toggled or modified state (for example, Caps Lock) for the input event.
		};

		enum class CoreProcessEventsOption : UINT32
		{
			ProcessOneAndAllIfPresent, // Dispatch all currently available events in the queue. If no events are pending, wait for the next new event.
			ProcessOneIfPresent, // Dispatch one event if it is currently pending in the queue. If no events are pending, do not wait for a new event to be raised but instead return immediately.
			ProcessUntilQuit, // Wait for new events and dispatch all available events. Continue this behavior until the window is closed or the application calls the Close method on the CoreWindow instance.
			ProcessAllIfPresent, // Dispatch all events currently pending in the queue. If no events are pending, do not wait for a new event to be raised but instead return immediately.
		};

		enum class PointerUpdateKind : UINT32
		{
			Other = 0x00, // Pointer updates not identified by other PointerUpdateKind values.
			LeftButtonPressed = 1 << 0, // Left button pressed.
			LeftButtonReleased = 1 << 1, // Left button released.
			RightButtonPressed = 1 << 2, // Right button pressed.
			RightButtonReleased = 1 << 3, // Right button released.
			MiddleButtonPressed = 1 << 4, // Middle button pressed.
			MiddleButtonReleased = 1 << 5, // Middle button released.
			XButton1Pressed = 1 << 6, // XBUTTON1 pressed.
			XButton1Released = 1 << 7, // XBUTTON1 released.
			XButton2Pressed = 1 << 8, // XBUTTON2 pressed.
			XButton2Released = 1 << 9, // XBUTTON2 released.
		};

		typedef Message2 Message;
		typedef VirtualKey2 VirtualKey;
		typedef VirtualKeyModifiers2 VirtualKeyModifiers;
	}
}

#endif // !__NENA_APPEVENTASSETS_INCLUDED__
