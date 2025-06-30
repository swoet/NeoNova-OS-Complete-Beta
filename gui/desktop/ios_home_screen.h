#ifndef IOS_HOME_SCREEN_H
#define IOS_HOME_SCREEN_H

#define MAX_APPS_ON_HOME_SCREEN 50
#define MAX_APPS_IN_DOCK 4

// Placeholder for an app icon representation
typedef struct {
    char* name;
    char* icon_path; // Path to icon image file
    // void (*launch_action)(); // Action to execute when app is launched
} AppIcon;

typedef struct {
    AppIcon apps[MAX_APPS_ON_HOME_SCREEN];
    int app_count;

    AppIcon dock_apps[MAX_APPS_IN_DOCK];
    int dock_app_count;

    char* wallpaper_path; // Path to wallpaper image
} IOSHomeScreen;

// Function to create the iOS-style Home Screen
IOSHomeScreen* IOSHomeScreen_Create();

// Function to add an app to the home screen grid
int IOSHomeScreen_AddApp(IOSHomeScreen* screen, const char* name, const char* icon_path);

// Function to add an app to the dock
int IOSHomeScreen_AddAppToDock(IOSHomeScreen* screen, const char* name, const char* icon_path);

// Function to set the wallpaper
void IOSHomeScreen_SetWallpaper(IOSHomeScreen* screen, const char* wallpaper_path);

// Function to render the home screen
// This would generate the necessary UI structure (grid, dock, icons)
void IOSHomeScreen_Render(IOSHomeScreen* screen);

// Function to destroy the home screen and free its resources
void IOSHomeScreen_Destroy(IOSHomeScreen* screen);

#endif // IOS_HOME_SCREEN_H
