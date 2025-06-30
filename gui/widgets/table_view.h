#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H

#define MAX_TABLE_VIEW_CELLS 100 // Example limit

// For now, a cell is just a string.
// Could be extended to a struct for more complex cells.
typedef char* TableViewCellData;

typedef struct {
    TableViewCellData cells[MAX_TABLE_VIEW_CELLS];
    int cell_count;
    // void (*on_cell_select_callback)(int index, TableViewCellData data);
} TableView;

// Function to create a new table view
TableView* TableView_Create();

// Function to add a cell (string) to the table view
// Returns 0 on success, -1 on failure
int TableView_AddCell(TableView* view, const char* cell_text);

// Function to render the table view
void TableView_Render(TableView* view);

// Function to destroy the table view and free memory
void TableView_Destroy(TableView* view);

#endif // TABLE_VIEW_H
