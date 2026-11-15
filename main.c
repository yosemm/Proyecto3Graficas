#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define ANCHO 640
#define ALTO 360
#define PIXELES (ANCHO * ALTO)
#define ESCALA 2


Color colores[PIXELES];


typedef struct
{
    Vector3 position;
    Vector3 forward;
    Vector3 right;
    Vector3 up;
} CameraRay;

CameraRay cam;





void cameraLook(CameraRay *camera, Vector3 target)
{
    camera->forward = Vector3Normalize(Vector3Subtract(target, camera->position));
    camera->right = Vector3Normalize(Vector3CrossProduct(camera->forward, (Vector3){0, 1, 0}));
    camera->up = Vector3Normalize(Vector3CrossProduct(camera->right, camera->forward));
}

Color vectorColor(Vector3 c)
{
    c.x = Clamp(c.x, 0, 1);
    c.y = Clamp(c.y, 0, 1);
    c.z = Clamp(c.z, 0, 1);
    return (Color){(unsigned char)(c.x * 255), (unsigned char)(c.y * 255), (unsigned char)(c.z * 255), 255};
}






Vector3 cielo(Vector3 direccion)
{
    return (Vector3){0.02f, 0.02f, 0.06f};
}



Vector3 castRay(Vector3 origen, Vector3 direccion)
{
    return cielo(direccion);
}




int main(void)
{
    const int screenWidth = ANCHO * ESCALA;
    const int screenHeight = ALTO * ESCALA;

    InitWindow(screenWidth, screenHeight, "Proyecto 3 Space Travel");
    SetTargetFPS(60);

    float mitad = tanf(60 * DEG2RAD / 2);
    float aspecto = (float)ANCHO / ALTO;

    cam.position = (Vector3){0, 1, 10};


    Image imagenNegro = {
        .data = colores,
        .width = ANCHO,
        .height = ALTO,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
    Texture2D texturaColores = LoadTextureFromImage(imagenNegro);






    while (!WindowShouldClose())
    {
        Vector3 frente = {0, 0, -1};

        cameraLook(&cam, Vector3Add(cam.position, frente));


        for (int y = 0; y < ALTO; y++)
        {
            for (int x = 0; x < ANCHO; x++)
            {
                int indice = ((y * ANCHO) + x);
                float pantallax = ((x + 0.5f) / ANCHO * 2 - 1) * aspecto * mitad;
                float pantallay = (1 - (y + 0.5f) / ALTO * 2) * mitad;
                Vector3 direccion = Vector3Normalize(Vector3Add(Vector3Add(cam.forward, Vector3Scale(cam.right, pantallax)), Vector3Scale(cam.up, pantallay)));
                colores[indice] = vectorColor(castRay(cam.position, direccion));
            }
        }


        UpdateTexture(texturaColores, colores);
        BeginDrawing();
        DrawTextureEx(texturaColores, (Vector2){0, 0}, 0, ESCALA, WHITE);
        EndDrawing();
    }



    UnloadTexture(texturaColores);
    CloseWindow();
    return 0;
}
