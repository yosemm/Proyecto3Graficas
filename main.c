#include "raylib.h"

#define ANCHO 640
#define ALTO 360
#define PIXELES (ANCHO * ALTO)
#define ESCALA 2

Color colores[PIXELES];

int main(void)
{
    InitWindow(ANCHO * ESCALA, ALTO * ESCALA, "Proyecto 3 Space Travel");
    SetTargetFPS(60);
    for (int i = 0; i < PIXELES; i++)
        colores[i] = BLACK;
    Image imagenNegro = {
        .data = colores,
        .width = ANCHO,
        .height = ALTO,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
    Texture2D texturaColores = LoadTextureFromImage(imagenNegro);
    while (!WindowShouldClose())
    {
        UpdateTexture(texturaColores, colores);
        BeginDrawing();
        DrawTextureEx(texturaColores, (Vector2){0, 0}, 0, ESCALA, WHITE);
        EndDrawing();
    }
    UnloadTexture(texturaColores);
    CloseWindow();
    return 0;
}
