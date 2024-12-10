#include "raylib.h"
#include "raygui.h"

void show_main_menu(int width, int height)
{
    GuiPanel((Rectangle){0, 0, width, height}, "Simulation Parameters");
    
    const char *listItems = "Naive;Barnes Hut;PM";
    int active = 0;  // Currently selected item
    int scrollIndex = 0;
    GuiListView( (Rectangle){ 0, 25, width, 100 }, listItems, &scrollIndex, &active);
}

