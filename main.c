#include "raylib.h"
#include "raymath.h"
#include <math.h>

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





typedef struct
{
    Vector3 centro;
    float radio;
    Color *pixelesTextura;
    int anchoTextura;
    int altoTextura;
} Esfera;

#define NUM_ESFERAS 5

Esfera esferas[NUM_ESFERAS] = {
    {.centro = {0, 0, 0}, .radio = 1.5f},
    {.centro = {4, 0, 0}, .radio = 0.9f},
    {.centro = {-3.5f, 0, 3}, .radio = 1.0f},
    {.centro = {2.5f, 0, -5.5f}, .radio = 0.7f},
    {.centro = {-1.8f, 0, 4.2f}, .radio = 0.3f},
};
const char *archivosTextura[NUM_ESFERAS] = {"texturas/suntexture.png", "texturas/venus.png", "texturas/earth.png", "texturas/mars.png", "texturas/moontexture.png"};



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

RayCollision colisionEsfera(Vector3 origen, Vector3 direccion, Vector3 centro, float radio)
{
    RayCollision choque = {0};

    Vector3 oc = Vector3Subtract(origen, centro);
    float b = Vector3DotProduct(oc, direccion);
    float c = Vector3DotProduct(oc, oc) - radio * radio;
    float discriminante = b * b - c;

    if (discriminante < 0)
    {
        return choque;
    }

    float entrada = -b - sqrtf(discriminante);
    if (entrada < 0)
    {
        return choque;
    }

    choque.hit = true;
    choque.distance = entrada;
    choque.point = Vector3Add(origen, Vector3Scale(direccion, entrada));
    choque.normal = Vector3Normalize(Vector3Subtract(choque.point, centro));

    return choque;
}

Vector3 castRay(Vector3 origen, Vector3 direccion)
{

    RayCollision choque = {0};
    choque.distance = 1000.0f;
    int indice = -1;
    for (int i = 0; i < NUM_ESFERAS; i++)
    {
        RayCollision colision = colisionEsfera(origen, direccion, esferas[i].centro, esferas[i].radio);
        if (colision.hit && colision.distance < choque.distance)
        {
            choque = colision;
            indice = i;
        }
    }

    if (indice == -1)
    {
        return cielo(direccion);
    }

    Esfera esfera = esferas[indice];


    float u = 0.5f + atan2f(-choque.normal.z, choque.normal.x) / (2 * PI);
    float v = 0.5f - asinf(choque.normal.y) / PI;
    int px = (int)(u * (esfera.anchoTextura - 1));
    int py = (int)(v * (esfera.altoTextura - 1));
    int indiceTextura = py * esfera.anchoTextura + px;

    Vector3 colorTextura = {
        esfera.pixelesTextura[indiceTextura].r / 255.0f,
        esfera.pixelesTextura[indiceTextura].g / 255.0f,
        esfera.pixelesTextura[indiceTextura].b / 255.0f};

    return colorTextura;
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


    for (int i = 0; i < NUM_ESFERAS; i++)
    {
        Image imagen = LoadImage(archivosTextura[i]);
        if (imagen.data == 0)
        {
            CloseWindow();
            return 1;
        }
        esferas[i].anchoTextura = imagen.width;
        esferas[i].altoTextura = imagen.height;
        esferas[i].pixelesTextura = LoadImageColors(imagen);
        UnloadImage(imagen);
    }



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

    for (int i = 0; i < NUM_ESFERAS; i++)
    {
        UnloadImageColors(esferas[i].pixelesTextura);
    }

    UnloadTexture(texturaColores);
    CloseWindow();
    return 0;
}
