#include "raylib.h"

#define RAYGUI_IMPLEMENTATION

#include "raygui.h"

#include "rlgl.h"
#include "raymath.h"

#include "base.h"
#include "sim_naive.h"
#include "sim_barnes_hut.h"
#include "integrators.h"

#include <stdio.h>
#include <string.h> // For strncat
#include <stdlib.h>

#define MAX_STR_LEN 128
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAXCHAR 1024

typedef struct {
    int menu_w;
    int menu_h;
    bool open_body_creator;
    int sim_mode;
    int int_mode; 
    int scenario_mode;
    int scenario_current;
    char *sim_types;
    char *integrators;
    char *scenarios;
} MenuSettings;

typedef struct {
    float val;
    char *val_buffer;
    char *text_val;
    bool allow_edit;
} ValueBoxFloatControl;

typedef struct {
    int val;
    char *val_buffer;
    char *text_val;
    bool allow_edit;
} ValueBoxControl;

void load_celestial_data(Body* blist, int* bnum, int data_index) 
{
    FILE *fp = NULL;
    char row[MAXCHAR];

    if (data_index == 0) {
        fp = fopen(".\\celestial_data\\solar_system.csv","r");
    }
    else if (data_index == 1) {
        fp = fopen(".\\celestial_data\\binary_system.csv","r");
    }
    else if (data_index == 2) {
        fp = fopen(".\\celestial_data\\random_universe_256.csv","r");
    }

    fgets(row, MAXCHAR, fp);
    while (fgets(row, MAXCHAR, fp) != NULL)
    {   
        char *token;
        Body newbod = {0};

        token = strtok(row, ",");

        token = strtok(NULL, ","); // add code to handle names later

        newbod.mass = atof(token);
        token = strtok(NULL, ",");
        newbod.rad = atof(token);
        token = strtok(NULL, ",");
        newbod.pos.x = atof(token);
        token = strtok(NULL, ",");
        newbod.pos.y = atof(token);
        token = strtok(NULL, ",");
        newbod.vel.x = atof(token);
        token = strtok(NULL, ",");
        newbod.vel.y = atof(token);

        newbod.force = Vector2Zero();

        token = strtok(NULL, ",");
        
        newbod.color = GetColor((unsigned int)strtoul(token, NULL, 16));
        blist[(*bnum)++] = newbod;
    }
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main ()
{
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "[Raylib] N-Body Simulation");

    Camera2D camera = { 0 };
    camera.target = (Vector2){0.0f, 0.0f};
    camera.offset = (Vector2){SCREEN_WIDTH/2, 1.25*SCREEN_HEIGHT/2};
    camera.zoom = 0.75f;
    camera.rotation = 0.0f;

    Rectangle world_border = (Rectangle){-UNIVERSE_WIDTH/2, 
        -UNIVERSE_HEIGHT/2, UNIVERSE_WIDTH, UNIVERSE_HEIGHT};

    Color bgcolor = GetColor((unsigned int)0x2F3B52FF);

    bool spawn_mode = false;

    int bnum = 0;   
    Body *blist = calloc(MAX_NUM_BODIES, sizeof(Body));

    GuiLoadStyle("style_jungle.rgs");

    MenuSettings *mset = (MenuSettings *)malloc(sizeof(MenuSettings)); // Allocate memory
    mset->menu_w = GetScreenWidth();
    mset->menu_h = GetScreenHeight()*0.2;
    mset->open_body_creator = false;
    mset->sim_mode = 0;
    mset->int_mode = 0;
    mset->scenario_mode = 0;
    mset->scenario_current = 0;
    mset->sim_types = "Naive;Barnes Hut";
    mset->integrators = "Euler;Vel Verlet;Yoshida4";
    mset->scenarios = "Solar System;Binary System;Random Dist 256";

    ValueBoxFloatControl *mdt = (ValueBoxFloatControl *)malloc(sizeof(ValueBoxFloatControl));
    mdt->val = 0.1;
    mdt->val_buffer = (char *)calloc(MAX_STR_LEN, sizeof(char));         
    mdt->text_val = (char *)calloc(MAX_STR_LEN, sizeof(char));
    snprintf(mdt->text_val, MAX_STR_LEN, "%f", mdt->val); 
    mdt->allow_edit = false;  

    ValueBoxControl *bh_cap_setting = (ValueBoxControl *)malloc(sizeof(ValueBoxControl));
    bh_cap_setting->val = 3;
    bh_cap_setting->val_buffer = (char *)calloc(MAX_STR_LEN, sizeof(char));
    bh_cap_setting->text_val = (char *)calloc(MAX_STR_LEN, sizeof(char));
    bh_cap_setting->allow_edit = false;

    ValueBoxFloatControl *bh_theta_setting = (ValueBoxFloatControl *)malloc(sizeof(ValueBoxFloatControl));
    bh_theta_setting->val = 0.5;
    bh_theta_setting->val_buffer = (char *)calloc(MAX_STR_LEN, sizeof(char));
    bh_theta_setting->text_val = (char *)calloc(MAX_STR_LEN, sizeof(char));
    snprintf(bh_theta_setting->text_val, MAX_STR_LEN, "%f", bh_theta_setting->val); 
    bh_theta_setting->allow_edit = false;

    // Keeping these as separate variables to prevent situations where the 
    // GUI behaviour sets them to 0 or NULL
    int bh_carrying_capacity = bh_cap_setting->val;
    float bh_theta_val = bh_theta_setting->val;

    Integrator integrator = {EULER, integrate_euler};

    load_celestial_data(blist, &bnum, mset->scenario_current);

   //--------------------------------------------------------------------------------------

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
            DrawCircleV((Vector2){5.0f, 5.0f}, 5.0f, BLACK);
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

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && spawn_mode == true)
        {
            blist[bnum].pos = GetScreenToWorld2D(GetMousePosition(), camera);
            blist[bnum].vel = (Vector2){ x: 0.0, y: 0.0 };
            blist[bnum++].mass = 1;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && spawn_mode == true)
        {
            blist[bnum].pos = GetScreenToWorld2D(GetMousePosition(), camera);
            blist[bnum].vel = (Vector2){ x: 0.0, y: 0.0 };
            blist[bnum++].mass = 10000;
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

        integrator = select_integrator((IntegratorType) mset->int_mode);

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(bgcolor);
            BeginMode2D(camera);
                //----------------------------------------------------------------------------------
                // Naive Simulation
                //----------------------------------------------------------------------------------     
                if (mset->sim_mode == 0)
                {                
                    sim_naive(blist, bnum, mdt->val, integrator);        
                }
                //----------------------------------------------------------------------------------     
                else if (mset->sim_mode == 1)
                {
                    metadata_BH mbh = sim_barnes_hut(blist, bnum, bh_carrying_capacity, bh_theta_val, mdt->val, integrator);

                    Rectangle *rects = malloc((4*bnum+1) * sizeof(Rectangle));
                    Vector2 *cmasses = malloc((4*bnum+1) * sizeof(Vector2));
                    float *tmasses = malloc((4*bnum+1) * sizeof(float));
                    
                    int count = 0;

                    dfs_info(mbh.root, rects, cmasses, tmasses, &count);
                
                    for (int i = 1; i < count; i++)
                    {   
                        Color color1 = GetColor((unsigned int)strtoul("0xAAAAFFFF", NULL, 16));
                        Color color2 = GetColor((unsigned int)strtoul("0xAAAAFF30", NULL, 16));
                        DrawRectangleLinesEx(rects[i],1.0, color1);
                        DrawRectangleRec(rects[i], color2);
                    }
                    free(rects);
                    free(cmasses);
                    free(tmasses);
                }

                for (int i = 0; i < bnum; i++)
                {
                    DrawCircleV(blist[i].pos, blist[i].rad, blist[i].color);                    
                }
                DrawRectangleLinesEx(world_border, 5.0, RAYWHITE);
            EndMode2D();           

            if (spawn_mode == false) 
                { DrawText("Spawn Mode: OFF", 20, 20, 20, DARKGREEN); }
            else 
            {                
                DrawCircleV(GetMousePosition(), 1, DARKGRAY);  
            }
        

            //----------------------------------------------------------------------------------
            // Setup GUI menu elements
            // rendered last so they're on top of everything else
            //----------------------------------------------------------------------------------
            
            GuiPanel((Rectangle){0, 0, mset->menu_w, mset->menu_h}, NULL);        

            GuiPanel((Rectangle){mset->menu_w*0.25, 0, mset->menu_w*0.25, mset->menu_h}, "Simulation Type");

            GuiPanel((Rectangle){mset->menu_w*0.50, 0, mset->menu_w*0.25, mset->menu_h}, 
                "Integrator");

            GuiListView( (Rectangle){ mset->menu_w*0.25, 25, mset->menu_w*0.25, mset->menu_h - 25 }, 
                mset->sim_types, NULL, &mset->sim_mode);
            
            GuiListView( (Rectangle){ mset->menu_w*0.50, 25, mset->menu_w*0.25, 
                mset->menu_h - 25 }, mset->integrators, NULL, &mset->int_mode);

            //----------------------------------------------------------------------------------
            // Scenario Selector
            //----------------------------------------------------------------------------------
            GuiPanel((Rectangle){0, 0, mset->menu_w*0.25, mset->menu_h}, "Simulation Type");   
            
            GuiListView( (Rectangle){ 0, 25, mset->menu_w*0.25, mset->menu_h - 25 }, 
                mset->scenarios, NULL, &mset->scenario_mode);

            if (mset->scenario_mode != mset->scenario_current)
            {   
                free(blist);
                bnum = 0;
                blist = calloc(MAX_NUM_BODIES, sizeof(Body));
                load_celestial_data(blist, &bnum, mset->scenario_mode);
                mset->scenario_current = mset->scenario_mode;
            }


            //----------------------------------------------------------------------------------
            // Delta_t value controls
            //----------------------------------------------------------------------------------
            GuiPanel((Rectangle){mset->menu_w*0.75, 0, mset->menu_w*0.25, mset->menu_h*0.5}, 
                "Delta t");
                
            if (GuiValueBoxFloat(
                (Rectangle){ mset->menu_w*0.75, 25, mset->menu_w*0.25, mset->menu_h*0.5 - 25}, 
                NULL, mdt->text_val, &mdt->val, mdt->allow_edit))
            {
                mdt->allow_edit = !mdt->allow_edit;
            }
            snprintf(mdt->val_buffer, MAX_STR_LEN, "%f", mdt->val); 
            //----------------------------------------------------------------------------------
            // Body creation system
            //----------------------------------------------------------------------------------
            /*
            if (GuiButton(
                (Rectangle){mset->menu_w*0.75, mset->menu_h*0.5+25, mset->menu_w*0.25, mset->menu_h*0.5 - 25}, 
                "Open Menu")) 
            {
                mset->open_body_creator = true;
            }
            */
            //----------------------------------------------------------------------------------
            // Barnes Hut specific menu
            //----------------------------------------------------------------------------------
            if (mset->sim_mode == 1)
            {
                Rectangle bh_set = {0, SCREEN_HEIGHT-mset->menu_h, mset->menu_w*0.25, mset->menu_h};
                GuiPanel(bh_set, "Barnes Hut Settings");     

                GuiLabel((Rectangle){bh_set.x+15, bh_set.y+25,bh_set.width,bh_set.height*0.25}, "Node Carrying Cap.");

                // Getting errors if min val is 1, need to look into this
                if (GuiValueBox(
                    (Rectangle){ bh_set.x+150, bh_set.y+25, bh_set.width-150, bh_set.height*0.25}, 
                    NULL, &bh_cap_setting->val, 2, 100, bh_cap_setting->allow_edit))
                {
                    bh_cap_setting->allow_edit = !bh_cap_setting->allow_edit;

                    bh_carrying_capacity = bh_cap_setting->val;
                }

                GuiLabel((Rectangle){bh_set.x+15, bh_set.y+60,bh_set.width,bh_set.height*0.25}, "Theta Ratio Val.");

                if (GuiValueBoxFloat(
                    (Rectangle){ bh_set.x+150, bh_set.y+60, bh_set.width-150, bh_set.height*0.25}, 
                    NULL, bh_theta_setting->text_val, &bh_theta_setting->val, bh_theta_setting->allow_edit)
                )
                {
                    if (bh_theta_setting->val < 0.1) { bh_theta_setting->val = 0.1; }
                    else if (bh_theta_setting->val > 1.0) { bh_theta_setting->val = 1.0; }

                    snprintf(bh_theta_setting->text_val, MAX_STR_LEN, "%.2f", bh_theta_setting->val); 
                    bh_theta_val = bh_theta_setting->val;
                    bh_theta_setting->allow_edit = !bh_theta_setting->allow_edit;
                }

                //GuiLabel((Rectangle){bh_set.x+15, bh_set.y + 50,bh_set.width,bh_set.height*0.5}, "Display Grid");
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}