#ifndef NAVIGATION_BAR_H
#define NAVIGATION_BAR_H

// Structure for the Navigation Bar
typedef struct {
    char* title;
    // We can add more properties later, like left/right buttons
} NavigationBar;

// Function to create a new navigation bar
NavigationBar* NavigationBar_Create(const char* title);

// Function to set the title of the navigation bar
void NavigationBar_SetTitle(NavigationBar* bar, const char* title);

// Function to render the navigation bar
// This would typically involve generating HTML/markup with classes from ios_theme.css
void NavigationBar_Render(NavigationBar* bar);

// Function to destroy the navigation bar and free memory
void NavigationBar_Destroy(NavigationBar* bar);

#endif // NAVIGATION_BAR_H
