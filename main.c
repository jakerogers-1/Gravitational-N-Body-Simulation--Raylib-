#include "raylib.h"

#define RAYGUI_IMPLEMENTATION

#include "raygui.h"

#include "rlgl.h"
#include "raymath.h"

#include <stdio.h>
#include <string.h> // For strncat
#include <stdlib.h>

#include "layout.h"


typedef struct {
    Vector2 pos;
    Vector2 vel; 
    float mass;
} Body;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main ()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera mouse zoom");

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    bool spawn_mode = false;

    int spawn_sizes[] = {1, 2, 3, 4, 5};
    int spawn_index = 0;

    const int MAX_BODIES = 1024;
    int bnum = 0;
    Body *blist = calloc(MAX_BODIES, sizeof(Body));

    const int GRAV_CONST = 1;
    float delta_t = 0.1;
    bool buttonClicked = false;

    GuiLoadStyle("style_jungle.rgs");

    bool show_readme_window = true;

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        buttonClicked = GuiButton((Rectangle){ 100, 100, 120, 40 }, "Click Me");
        if (buttonClicked) 
        {
            DrawText("Button clicked!", 250, 110, 20, DARKGRAY);
        }

        if (show_readme_window && GuiWindowBox((Rectangle){50, 50, 250, 250}, "Read Me"))
        {
            show_readme_window = false;
        }
        
        //GuiPanel((Rectangle){0, 0, 200, 600}, "Simulation Settings"); 
        show_main_menu(GetScreenWidth()*0.2, GetScreenHeight());


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
                rlPushMatrix();
                    rlTranslatef(0, 25*50, 0);
                    rlRotatef(90, 1, 0, 0);
                rlPopMatrix();

                // Draw a reference circle
                DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, 50, MAROON);

            for (int i = 0; i < bnum; i++) 
            {
                Vector2 net_force = Vector2Zero();
                Vector2 r1 = blist[i].pos;

                for (int j = 0; j < bnum; j++) 
                {
                    if (i == j) { continue; }
                    
                    Vector2 r2 = blist[j].pos;

                    Vector2 force = Vector2Scale(Vector2Normalize(Vector2Subtract(r2,r1)), 
                    GRAV_CONST * blist[i].mass * blist[j].mass / Vector2DistanceSqr(r1, r2));

                    net_force = Vector2Add(net_force, force);
                }

                blist[i].vel = Vector2Add(blist[i].vel, Vector2Scale(net_force, delta_t / blist[i].mass));
                blist[i].pos = Vector2Add(blist[i].pos, Vector2Scale(blist[i].vel, delta_t));
                
                DrawCircle(
                    blist[i].pos.x, blist[i].pos.y, 4*blist[i].mass, BLUE
                );
            }
            
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

                DrawCircleV(GetMousePosition(), 4*spawn_sizes[spawn_index], DARKGRAY);  
            }
            


            
            // Draw mouse reference
            //Vector2 mousePos = GetWorldToScreen2D(GetMousePosition(), camera)
            //DrawCircleV(GetMousePosition(), 4, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}