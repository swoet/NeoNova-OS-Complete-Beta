#include "alert.h"
#include <stdlib.h> // For malloc, free
#include <string.h> // For strdup, memset
#include <stdio.h>  // For printf (used for conceptual rendering)

// Function to create a new alert
Alert* Alert_Create(const char* title, const char* message) {
    Alert* alert = (Alert*)malloc(sizeof(Alert));
    if (!alert) {
        return NULL;
    }
    alert->title = title ? strdup(title) : NULL;
    alert->message = message ? strdup(message) : NULL;
    alert->action_count = 0;
    // alert->is_visible = 0; // Initially hidden
    memset(alert->actions, 0, sizeof(alert->actions));
    return alert;
}

// Function to add an action to the alert
int Alert_AddAction(Alert* alert, const char* action_title, AlertActionStyle style, void (*handler)()) {
    if (!alert || alert->action_count >= MAX_ALERT_ACTIONS) {
        return -1; // Error: alert is null or max actions reached
    }

    alert->actions[alert->action_count].title = action_title ? strdup(action_title) : NULL;
    alert->actions[alert->action_count].style = style;
    alert->actions[alert->action_count].handler = handler;

    alert->action_count++;
    return 0; // Success
}

// Function to show (render) the alert
void Alert_Show(Alert* alert) {
    if (!alert) return;
    // alert->is_visible = 1;

    // Conceptual: Output HTML-like structure with classes from ios_theme.css
    printf("<div class=\"alert\">\n");
    if (alert->title && strlen(alert->title) > 0) {
        printf("  <div class=\"alert-title\">%s</div>\n", alert->title);
    }
    if (alert->message && strlen(alert->message) > 0) {
        printf("  <div class=\"alert-message\">%s</div>\n", alert->message);
    }
    if (alert->action_count > 0) {
        printf("  <div class=\"alert-actions\">\n");
        for (int i = 0; i < alert->action_count; ++i) {
            const char* style_class = "";
            if (alert->actions[i].style == ALERT_ACTION_STYLE_DESTRUCTIVE) {
                style_class = " alert-button-destructive";
            } else if (alert->actions[i].style == ALERT_ACTION_STYLE_CANCEL) {
                // Cancel buttons might have specific styling or positioning in a full impl.
                // For now, they use default button style.
            }
            // Conceptual: In a real system, clicking this would trigger alert->actions[i].handler()
            printf("    <div class=\"alert-button%s\">%s</div>\n", style_class, alert->actions[i].title);
        }
        printf("  </div>\n");
    }
    printf("</div>\n");
}

// Function to destroy the alert and free memory
void Alert_Destroy(Alert* alert) {
    if (!alert) return;
    if (alert->title) {
        free(alert->title);
    }
    if (alert->message) {
        free(alert->message);
    }
    for (int i = 0; i < alert->action_count; ++i) {
        if (alert->actions[i].title) {
            free(alert->actions[i].title);
        }
    }
    free(alert);
}
