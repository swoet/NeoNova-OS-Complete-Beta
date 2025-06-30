#include "table_view.h"
#include <stdlib.h> // For malloc, free
#include <string.h> // For strdup, memset
#include <stdio.h>  // For printf (used for conceptual rendering)

// Function to create a new table view
TableView* TableView_Create() {
    TableView* view = (TableView*)malloc(sizeof(TableView));
    if (!view) {
        return NULL;
    }
    view->cell_count = 0;
    // view->on_cell_select_callback = NULL;
    memset(view->cells, 0, sizeof(view->cells));
    return view;
}

// Function to add a cell (string) to the table view
int TableView_AddCell(TableView* view, const char* cell_text) {
    if (!view || view->cell_count >= MAX_TABLE_VIEW_CELLS) {
        return -1; // Error: view is null or max cells reached
    }

    view->cells[view->cell_count] = cell_text ? strdup(cell_text) : NULL;
    if (cell_text && !view->cells[view->cell_count]) {
        // strdup failed
        return -1;
    }
    view->cell_count++;
    return 0; // Success
}

// Function to render the table view
void TableView_Render(TableView* view) {
    if (!view) return;

    // Conceptual: Output HTML-like structure with classes from ios_theme.css
    printf("<div class=\"table-view\">\n");
    for (int i = 0; i < view->cell_count; ++i) {
        printf("  <div class=\"table-view-cell\">\n");
        if (view->cells[i]) {
            printf("    %s\n", view->cells[i]);
        }
        printf("  </div>\n");
    }
    printf("</div>\n");
}

// Function to destroy the table view and free memory
void TableView_Destroy(TableView* view) {
    if (!view) return;
    for (int i = 0; i < view->cell_count; ++i) {
        if (view->cells[i]) {
            free(view->cells[i]);
        }
    }
    free(view);
}
