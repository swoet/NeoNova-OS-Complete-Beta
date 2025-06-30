#ifndef ALERT_H
#define ALERT_H

#define MAX_ALERT_ACTIONS 3 // Typically 1 to 3 actions

typedef enum {
    ALERT_ACTION_STYLE_DEFAULT,
    ALERT_ACTION_STYLE_CANCEL,      // Often styled differently or placed specifically
    ALERT_ACTION_STYLE_DESTRUCTIVE  // For actions like delete, logout
} AlertActionStyle;

typedef struct {
    char* title;
    AlertActionStyle style;
    void (*handler)(); // Function to call when action is triggered
} AlertAction;

typedef struct {
    char* title;
    char* message;
    AlertAction actions[MAX_ALERT_ACTIONS];
    int action_count;
    // int is_visible; // To control rendering
} Alert;

// Function to create a new alert
Alert* Alert_Create(const char* title, const char* message);

// Function to add an action to the alert
// Returns 0 on success, -1 on failure
int Alert_AddAction(Alert* alert, const char* action_title, AlertActionStyle style, void (*handler)());

// Function to show (render) the alert
// In a real system, this would make the alert visible and bring it to the front.
void Alert_Show(Alert* alert);

// Function to destroy the alert and free memory
void Alert_Destroy(Alert* alert);

#endif // ALERT_H
