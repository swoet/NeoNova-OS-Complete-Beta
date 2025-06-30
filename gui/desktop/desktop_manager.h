#ifndef DESKTOP_MANAGER_H
#define DESKTOP_MANAGER_H

// Forward declare if necessary, or include specific headers
// For now, assume ios_home_screen.h might be needed by users of DesktopManager_Initialize
// or for related types, though not strictly for the function signatures below.
// #include "ios_home_screen.h"

// --- UI Mode Concept ---
typedef enum {
    UI_MODE_UNKNOWN,
    UI_MODE_MOBILE,  // Compact, touch-first
    UI_MODE_TABLET,  // Larger touch interface
    UI_MODE_DESKTOP  // Mouse/keyboard focused, larger screens
} UIMode;

void DesktopManager_SetCurrentUIMode(UIMode mode);
UIMode DesktopManager_GetCurrentUIMode();
// --- End UI Mode Concept ---

// Placeholder for the GUI system's theme loading function
// This might be better placed in a dedicated GUISystem header if it grows.
void GUISystem_LoadTheme(const char* css_file_path);

// Initializes the desktop environment with a specific UI mode.
void DesktopManager_Initialize(UIMode initial_mode);

// Shuts down the desktop manager and cleans up resources.
void DesktopManager_Shutdown();

// Conceptual function to trigger a re-render or UI update if mode changes dynamically
void DesktopManager_NotifyUIModeChanged();


#endif // DESKTOP_MANAGER_H
