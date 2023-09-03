

/*******************************************************************************************
*
*   raylib [models] example - Models loading
*
*   NOTE: raylib supports multiple models file formats:
*
*     - OBJ  > Text file format. Must include vertex position-texcoords-normals information,
*              if files references some .mtl materials file, it will be loaded (or try to).
*     - GLTF > Text/binary file format. Includes lot of information and it could
*              also reference external files, raylib will try loading mesh and materials data.
*     - IQM  > Binary file format. Includes mesh vertex data but also animation data,
*              raylib can load .iqm animations.
*     - VOX  > Binary file format. MagikaVoxel mesh format:
*              https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt
*     - M3D  > Binary file format. Model 3D format:
*              https://bztsrc.gitlab.io/model3d
*
*   Example originally created with raylib 2.0, last time updated with raylib 4.2
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2014-2023 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - models loading");

    bool spin = true;        // Spin the camera?

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 100.0f, 60.0f, 80.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 10.0f, 0.0f };     // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;                   // Camera mode type
    int camera_mode = CAMERA_ORBITAL;

    // Inside Brain
    Model model_intern = LoadModel("9_brain/source/brain.obj");
    Texture texture_intern = LoadTexture("9_brain/textures/BrainSpec.png");
    model_intern.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture_intern;            // Set map diffuse texture
    Vector3 position_intern = { 0.0f, -20.0f, -10.0f };                    // Set model position
    BoundingBox bounds_intern = GetMeshBoundingBox(model_intern.meshes[0]);   // Set model bounds

    // Outside Brain
    Model model = LoadModel("9_brain/source/test.obj"); // Load model
    Texture2D texture = LoadTexture("9_brain/textures/test_Albedo.png"); // Load model texture
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;            // Set map diffuse texture
    Vector3 position = { 10.0f, 10.0f, 10.0f };                    // Set model position
    BoundingBox bounds = GetMeshBoundingBox(model.meshes[0]);   // Set model bounds

    Ray ray = { 0 };                    // Picking line ray
    // NOTE: bounds are calculated from the original size of the model,
    // if model is scaled on drawing, bounds must be also scaled

    bool selected = false;          // Selected object flag

    DisableCursor();                // Limit cursor to relative movement inside the window

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsCursorHidden()) UpdateCamera(&camera, camera_mode);

        // Load new models/textures on drag&drop
        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            if (droppedFiles.count == 1) // Only support one file dropped
            {
                if (IsFileExtension(droppedFiles.paths[0], ".obj") ||
                    IsFileExtension(droppedFiles.paths[0], ".gltf") ||
                    IsFileExtension(droppedFiles.paths[0], ".glb") ||
                    IsFileExtension(droppedFiles.paths[0], ".vox") ||
                    IsFileExtension(droppedFiles.paths[0], ".iqm") ||
                    IsFileExtension(droppedFiles.paths[0], ".m3d"))       // Model file formats supported
                {
                    UnloadModel(model);                         // Unload previous model
                    model = LoadModel(droppedFiles.paths[0]);   // Load new model
                    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; // Set current map diffuse texture

                    bounds = GetMeshBoundingBox(model.meshes[0]);

                    // TODO: Move camera position from target enough distance to visualize model properly
                }
                else if (IsFileExtension(droppedFiles.paths[0], ".png"))  // Texture file formats supported
                {
                    // Unload current model texture and load new one
                    UnloadTexture(texture);
                    texture = LoadTexture(droppedFiles.paths[0]);
                    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
                }
            }

            UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
        }
        // Change camera mode
        if (IsKeyPressed(KEY_F3))
        {
            // Handle camera change
            spin = !spin;
            // we need to reset the camera when changing modes

            //Camera camera = { 0 };
            camera = (Camera3D){ 0 };
            camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };          // Camera looking at point
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };              // Camera up vector (rotation towards target)
            camera.fovy = 45.0f;                                    // Camera field-of-view Y
            camera.projection = CAMERA_PERSPECTIVE;                 // Camera mode type

            if (spin)
            {
                camera.position = (Vector3){ 100.0f, 60.0f, 80.0f }; // Camera position
                camera_mode = CAMERA_ORBITAL;
            }
            else
            {
                camera.position = (Vector3){ 60.0f, 50.0f, 80.0f }; // Camera position
                camera_mode = CAMERA_FREE;
            }
        }
        // Select model on mouse click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            ray = GetMouseRay(GetMousePosition(), camera);
            // Check collision between ray and box
            if (GetRayCollisionBox(ray, bounds).hit)
            {
                selected = !selected;
                camera = (Camera3D){ 0 };
                camera.position = (Vector3){ 60.0f, 50.0f, 80.0f }; // Camera position
                camera.target = (Vector3){ 0.0f, 10.0f, 0.0f };     // Camera looking at point
                camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
                camera.fovy = 45.0f;                                // Camera field-of-view Y
                camera.projection = CAMERA_PERSPECTIVE;                   // Camera mode type
                camera_mode = CAMERA_FREE;
            }
            else selected = false;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);

            BeginMode3D(camera);
                //DrawGrid(20, 10.0f);         // Draw a grid
                if (selected)
                  {

                    ClearBackground(GRAY);
                    DrawBoundingBox(bounds, GREEN);   // Draw selection box
                    DrawModel(model_intern, position_intern, 1.0f, WHITE);        // Draw 3d model with texture
                  }
                else {
                    DrawBoundingBox(bounds, RED);   // Draw selection box
                    DrawModel(model, position, 1.0f, GRAY);        // Draw 3d model with texture
                }
            EndMode3D();
            DrawRectangle(0, 0, screenWidth, 5, RED);
            DrawRectangle(0, 5, 5, screenHeight - 10, RED);
            DrawRectangle(screenWidth - 5, 5, 5, screenHeight - 10, RED);
            DrawRectangle(0, screenHeight - 5, screenWidth, 5, RED);

            DrawRectangle( 10, 10, 220, 150, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines( 10, 10, 220, 150, BLUE);

            DrawText("F3 Camara libre/Camara orbital", 30, 20, 10, WHITE);
            DrawText("Controles camara orbital:", 20, 40, 10, WHITE);
            DrawText("Rueda del raton para hacer Zoom", 30, 60, 10, WHITE);
            DrawText("Controles camara libre:", 20, 80, 10, WHITE);
            DrawText("A/W/S/D/ para mover la camara", 30, 100, 10, WHITE);
            DrawText("Q/E para rotar", 30, 120, 10, WHITE);
            DrawText("Click izquierdo para interactuar", 30, 140, 10, WHITE);


            DrawText("Acercate al cerebro y encuentra la entrada.", screenWidth - 350, GetScreenHeight() - 440, 15, WHITE);
            DrawText(TextFormat("PosX: %.2f", camera.position), screenWidth - 350, GetScreenHeight() - 420, 15, WHITE);
            DrawText("Juan Pablo Henao", screenWidth - 620, screenHeight - 20, 15, WHITE);
            DrawText("Valeria Ivania Florez", screenWidth - 320, screenHeight - 20, 15, WHITE);

            //DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);     // Unload texture
    UnloadModel(model);         // Unload model

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
