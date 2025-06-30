#ifndef IOS_HOME_SCREEN_H
#define IOS_HOME_SCREEN_H

// Note: AppIcon struct and direct app management are removed.
// The home screen will now get app information from AppManager.

typedef struct {
    // int current_page; // For future pagination
    char* wallpaper_path; // Path to wallpaper image
    // Other home screen specific settings, e.g., grid layout preferences
} IOSHomeScreen;

// Function to create the iOS-style Home Screen
IOSHomeScreen* IOSHomeScreen_Create();

// Function to set the wallpaper
void IOSHomeScreen_SetWallpaper(IOSHomeScreen* screen, const char* wallpaper_path);

// Function to render the home screen
// This would generate the necessary UI structure (grid, dock, icons)
// It will interact with AppManager to get the list of apps.
void IOSHomeScreen_Render(IOSHomeScreen* screen);

// Function to destroy the home screen and free its resources
void IOSHomeScreen_Destroy(IOSHomeScreen* screen);

#endif // IOS_HOME_SCREEN_H
