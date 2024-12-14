#include "raylib.h"

#define RAYGUI_IMPLEMENTATION

#include "raygui.h"

#include "rlgl.h"
#include "raymath.h"

#include "base.h"
#include "sim_naive.h"

#include <stdio.h>
#include <string.h> // For strncat
#include <stdlib.h>

#define MAX_STR_LEN 128
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef struct {
    int menu_w;
    int menu_h;
    bool show_readme;
    int sim_mode;
    int int_mode; 
    char *sim_types;
    char *integrators;
} MenuSettings;

typedef struct {
    float delta_t;
    char *delta_t_buffer;
    char *text_val;
    bool allow_edit;
} DeltaTControl;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main ()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "[Raylib] N-Body Simulation");

    Camera2D camera = { 0 };
    camera.target = (Vector2){UNIVERSE_WIDTH/2, UNIVERSE_HEIGHT/2};
    camera.offset = (Vector2){GetScreenWidth()/2, GetScreenHeight()/2};
    camera.zoom = 0.15f;
    

    bool spawn_mode = false;

    int spawn_sizes[] = {1, 2, 3, 4, 5};
    int spawn_index = 0;

    const int MAX_BODIES = 1024;
    int bnum = 0;   
    Body *blist = calloc(MAX_BODIES, sizeof(Body));

    GuiLoadStyle("style_jungle.rgs");

    MenuSettings *mset = (MenuSettings *)malloc(sizeof(MenuSettings)); // Allocate memory
    mset->menu_w = GetScreenWidth();
    mset->menu_h = GetScreenHeight()*0.2;
    mset->show_readme = false;
    mset->sim_mode = 0;
    mset->int_mode = 0;
    mset->sim_types = "Naive;Barnes Hut;PM";
    mset->integrators = "Euler;RK4";

    DeltaTControl *mdt = (DeltaTControl *)malloc(sizeof(DeltaTControl));
    mdt->delta_t = 0.1;
    mdt->delta_t_buffer = (char *)calloc(MAX_STR_LEN, sizeof(char));         
    mdt->text_val = (char *)calloc(MAX_STR_LEN, sizeof(char));
    snprintf(mdt->text_val, MAX_STR_LEN, "%f", mdt->delta_t); 
    mdt->allow_edit = false;  
    
    const char *readme_text = 
        "This is a simulation of the N-body problem for gravity.\n\n"
        "A number of different simulation types and numerical integrators have\nbeen made available \n\n"
        "Please contact me at jakerogers-1 on GitHub for any points of discussion";

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        
        // Translate based on mouse right click
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f/camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }

        if (IsKeyDown(KEY_W))
        {
            Vector2 delta = (Vector2){0.0f, -15.0f};
            camera.target = Vector2Add(camera.target, delta);
        }

        if (IsKeyDown(KEY_S))
        {
            Vector2 delta = (Vector2){0.0f, 15.0f};
            camera.target = Vector2Add(camera.target, delta);
        }

        if (IsKeyDown(KEY_A))
        {
            Vector2 delta = (Vector2){-15.0f, 0.0f};
            camera.target = Vector2Add(camera.target, delta);
        }

        if (IsKeyDown(KEY_D))
        {
            Vector2 delta = (Vector2){15.0f, 0.0f};
            camera.target = Vector2Add(camera.target, delta);
        }

        if (IsKeyPressed(KEY_TAB))
        {
            if (spawn_mode == false) { spawn_mode = true; }
            else { spawn_mode = false; }
        }

        if (IsKeyPressed(KEY_Q) && spawn_mode == true)
        {
            if (spawn_index < (sizeof(spawn_sizes)/sizeof(spawn_sizes[0]) - 1)) 
            { spawn_index++; }
            else { spawn_index = 0; }
        }

        if (IsKeyPressed(KEY_E) && spawn_mode == true)
        {
            if (spawn_index > 0 ) { spawn_index--; }
            else { spawn_index = sizeof(spawn_sizes)/sizeof(spawn_sizes[0]) - 1; }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && spawn_mode == true)
        {
            blist[bnum].pos = GetScreenToWorld2D(GetMousePosition(), camera);
            blist[bnum].vel = (Vector2){ x: 0.0, y: 0.0 };
            blist[bnum++].mass = spawn_sizes[spawn_index];
        }

        // Zoom based on mouse wheel
        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            // Get the world point that is under the mouse
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            // Set the offset to where the mouse is
            camera.offset = GetMousePosition();

            // Set the target to match, so that the camera maps the world space point 
            // under the cursor to the screen space point under the cursor at any zoom
            camera.target = mouseWorldPos;

            // Zoom increment
            float scaleFactor = 1.0f + (0.25f*fabsf(wheel));
            if (wheel < 0) scaleFactor = 1.0f/scaleFactor;
            camera.zoom = Clamp(camera.zoom*scaleFactor, 0.125f, 64.0f);
        }
    
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode2D(camera);

                // Draw the 3d grid, rotated 90 degrees and centered around 0,0 
                // just so we have something in the XY plane
                

                // Draw a reference circle
                DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, 50, MAROON);

                DrawRectangleLinesEx(
                    (Rectangle){
                    -UNIVERSE_WIDTH/2, -UNIVERSE_HEIGHT/2, 
                    UNIVERSE_WIDTH, UNIVERSE_HEIGHT}, 15.0, RED);

                //----------------------------------------------------------------------------------
                // Naive Simulation
                //----------------------------------------------------------------------------------     
                if (mset->sim_mode == 0)
                {
                    sim_naive(blist, bnum, mdt->delta_t);
                    for (int i = 0; i < bnum; i++)
                    {
                    DrawCircle(
                            blist[i].pos.x, blist[i].pos.y, 4*blist[i].mass, BLUE
                        );
                    }
                }
                //----------------------------------------------------------------------------------     

            EndMode2D();

            if (spawn_mode == false) 
                { DrawText("Spawn Mode: OFF", 20, 20, 20, DARKGREEN); }
            else 
            {
                char mass_str[20]; 
                snprintf(mass_str, 
                    sizeof(mass_str), "%d", spawn_sizes[spawn_index]); // Convert the integer to a string
                char base[50] = "Spawn Mode: ON\nMass: 10^";
                strncat(base, mass_str, sizeof(base) - strlen(base) - 1);
                DrawText(base,  20, 20, 20, GREEN); 

                DrawCircleV(GetMousePosition(), 1*spawn_sizes[spawn_index], DARKGRAY);  
            }

            //----------------------------------------------------------------------------------
            // Setup GUI menu elements
            // rendered last so they're on top of everything else
            //----------------------------------------------------------------------------------
            GuiPanel((Rectangle){0, 0, mset->menu_w, mset->menu_h}, NULL);        

            GuiPanel((Rectangle){0, 0, mset->menu_w*0.15, mset->menu_h}, "Simulation Type");

            GuiPanel((Rectangle){mset->menu_w*0.15, 0, mset->menu_w*0.15, mset->menu_h}, 
                "Integrator");

            GuiListView( (Rectangle){ 0, 25, mset->menu_w*0.15, mset->menu_h - 25 }, 
                mset->sim_types, NULL, &mset->sim_mode);
            
            GuiListView( (Rectangle){ mset->menu_w*0.15, 25, mset->menu_w*0.15, 
                mset->menu_h - 25 }, mset->integrators, NULL, &mset->int_mode);

            if (mset->show_readme)
            {
                if (GuiWindowBox(
                (Rectangle){ GetScreenWidth()*0.4, GetScreenHeight()*0.4, 
                GetScreenWidth()*0.2, GetScreenHeight()*0.2 }, "Read Me"))
                {
                    mset->show_readme = false;
                }
                
                GuiTextBox((Rectangle){ GetScreenWidth()*0.25, GetScreenHeight()*0.25, 
                GetScreenWidth()*0.50, GetScreenHeight()*0.50 },(char *)readme_text, 64, false); 
            }
            //----------------------------------------------------------------------------------
            // Delta_t value controls
            //----------------------------------------------------------------------------------
            GuiPanel((Rectangle){mset->menu_w*0.3, 0, mset->menu_w*0.15, mset->menu_h}, 
                "Delta t");
                
            if (GuiValueBoxFloat(
                (Rectangle){ mset->menu_w*0.3, 25, mset->menu_w*0.15, mset->menu_h*0.3 }, 
                NULL, mdt->text_val, &mdt->delta_t, mdt->allow_edit))
            {
                mdt->allow_edit = !mdt->allow_edit;
            }

            snprintf(mdt->delta_t_buffer, MAX_STR_LEN, "%f", mdt->delta_t);            
            //----------------------------------------------------------------------------------
            //
            //----------------------------------------------------------------------------------


        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}