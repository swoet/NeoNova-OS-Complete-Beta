#include "gui/desktop/desktop_manager.h"
#include "cli/cli.h" // Include the new CLI header
#include <stdio.h>

// This is a simplified main function for a test harness.
// In a real OS, GUI initialization and the main loop would be much more complex
// and integrated with the kernel, scheduler, input systems, etc.

int main() {
    printf("Starting Test Harness for NeoNovaOS...\n\n");

    // Initialize the Desktop Manager, which should set up the theme and home screen
    // This will print the conceptual HTML-like output via the *_Render functions.
    // We can test different modes by changing the argument here.
    DesktopManager_Initialize(UI_MODE_MOBILE); // Or UI_MODE_TABLET, UI_MODE_DESKTOP

    // In a real application, there would be an event loop here.
    // For this test harness, we just initialize and conceptually render once.
    // To test mode switching:
    // printf("\n\n--- SWITCHING TO DESKTOP MODE ---\n\n");
    // DesktopManager_SetCurrentUIMode(UI_MODE_DESKTOP);
    // // DesktopManager_NotifyUIModeChanged(); // Already called by SetCurrentUIMode if mode changes

    // printf("\n\n--- SWITCHING TO MOBILE MODE ---\n\n");
    // DesktopManager_SetCurrentUIMode(UI_MODE_MOBILE);

    // Example of how other widgets might be used and rendered (conceptually)
    // This part is commented out because DesktopManager_Initialize already renders the home screen.
    // If we wanted to test individual widgets, we'd call their Create/Render functions here.
    /*
    printf("\n--- Conceptual NavigationBar Render Start ---\n");
    NavigationBar* navBar = NavigationBar_Create("My App");
    if (navBar) {
        NavigationBar_Render(navBar);
        NavigationBar_Destroy(navBar);
    }
    printf("--- Conceptual NavigationBar Render End ---\n");

    printf("\n--- Conceptual TabBar Render Start ---\n");
    TabBar* tabBar = TabBar_Create();
    if (tabBar) {
        TabBar_AddItem(tabBar, "Home");
        TabBar_AddItem(tabBar, "Explore");
        TabBar_AddItem(tabBar, "Profile");
        TabBar_SetActiveItem(tabBar, 1);
        TabBar_Render(tabBar);
        TabBar_Destroy(tabBar);
    }
    printf("--- Conceptual TabBar Render End ---\n");
    */

    // --- Initialize and Test CLI ---
    printf("\n\n--- CLI Initialization and Test ---\n");
    CLI_Initialize(); // Initialize CLI and register commands

    // Simulate some CLI inputs
    CLI_ProcessInput("help");
    CLI_ProcessInput("echo Hello from the CLI!");
    CLI_ProcessInput("list_apps");
    CLI_ProcessInput("start_app com.neonovos.messages"); // Try to start one of the sample apps
    CLI_ProcessInput("start_app com.nonexistent.app");   // Try to start a non-existent app
    CLI_ProcessInput("unknown_command with args");
    CLI_ProcessInput(""); // Empty input
    CLI_ProcessInput("echo"); // Echo with no args
    printf("--- End CLI Test ---\n\n");

    // Shutdown the Desktop Manager to free resources
    DesktopManager_Shutdown();

    printf("\nTest Harness finished.\n");
    return 0;
}

// To manually test (conceptual steps):
// 1. Compile this main_test_harness.c file along with all the C files from:
//    - gui/desktop/desktop_manager.c
//    - gui/desktop/ios_home_screen.c
//    - gui/widgets/navigation_bar.c
//    - gui/widgets/tab_bar.c
//    - gui/widgets/table_view.c
//    - gui/widgets/alert.c
//    Ensure all include paths are correctly set up for the compiler.
//    Example (very basic, might need more flags for a real C project):
//    gcc -I. main_test_harness.c gui/desktop/*.c gui/widgets/*.c -o ui_test
//
// 2. Run the compiled executable and redirect its output to an HTML file:
//    ./ui_test > test_output.html
//
// 3. Create a simple wrapper HTML file (e.g., `viewer.html`) like this:
//    <!DOCTYPE html>
//    <html>
//    <head>
//        <title>NeoNovaOS UI Test</title>
//        <link rel="stylesheet" href="gui/theme/ios_theme.css">
//        <style>
//            body { background-color: #333; padding: 20px; }
//            /* Add any additional test page styling here */
//        </style>
//    </head>
//    <body>
//        <!-- Content will be injected here by replacing this comment -->
//        <!-- PASTE_HTML_OUTPUT_HERE -->
//    </body>
//    </html>
//
// 4. Open `test_output.html`, copy its entire content.
// 5. Open `viewer.html` in a text editor, and paste the copied content
//    in place of `<!-- PASTE_HTML_OUTPUT_HERE -->`.
// 6. Save `viewer.html` and open it in a web browser to see the visual representation.
//    (Ensure gui/theme/ios_theme.css is in the correct relative path to viewer.html)
//
// This process allows visual inspection of the printf-based rendering combined with the CSS.
// Iteration would involve modifying C code or CSS, recompiling, and refreshing the browser.
//
// Asset paths (icons, wallpapers) in the C code and CSS are placeholders.
// For them to work in this test, actual image files would need to exist at those paths,
// or the paths would need to be adjusted to point to available test images.
// For example, you might need to create dummy files like:
// gui/assets/wallpapers/default_ios_wallpaper.png
// gui/assets/icons/messages.png
// etc.
// Or, for testing, use online placeholder image services if the browser can access them.
