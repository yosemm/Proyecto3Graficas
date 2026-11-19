#include "raylib.h"
#include "raymath.h"
#include <math.h>

#define ANCHO 640
#define ALTO 360
#define PIXELES (ANCHO * ALTO)
#define ESCALA 2

// Array de pixeles a pintar
Color colores[PIXELES];

// Mi struct para la camara
typedef struct
{
    Vector3 position;
    Vector3 forward;
    Vector3 right;
    Vector3 up;
} CameraRay;

CameraRay cam;

Vector3 posLuz = {0, 0, 0};
// El sol es mi fuente para iluminar los planetas

// Struct para las esferas
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
    {.centro = {0, 0, 0}, .radio = 1.5f},        // Sol
    {.centro = {4, 0, 0}, .radio = 0.9f},        // Venus
    {.centro = {-3.5f, 0, 3}, .radio = 1.0f},    // Tierra
    {.centro = {2.5f, 0, -5.5f}, .radio = 0.7f}, // Marte
    {.centro = {-1.8f, 0, 4.2f}, .radio = 0.3f}, // Luna
};
const char *archivosTextura[NUM_ESFERAS] = {"texturas/suntexture.png", "texturas/venus.png", "texturas/earth.png", "texturas/mars.png", "texturas/moontexture.png"};

Color *pixelesCielo;
int anchoCielo;
int altoCielo;

// Para volver a implementar la camara y las colisiones de los rayos me apoyé de mi proyecto 2, pero para
// entender la formula de colisiones con esferas me fue de apoyo Claude.
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

// Skybox usando la textura de estrellas
Vector3 cielo(Vector3 direccion)
{
    float u = 0.5f + atan2f(direccion.z, direccion.x) / (2 * PI);
    float v = 0.5f - asinf(direccion.y) / PI;
    int px = (int)(u * (anchoCielo - 1));
    int py = (int)(v * (altoCielo - 1));
    int indiceCielo = py * anchoCielo + px;

    Vector3 fondo = {
        pixelesCielo[indiceCielo].r / 255.0f,
        pixelesCielo[indiceCielo].g / 255.0f,
        pixelesCielo[indiceCielo].b / 255.0f};

    float horizonte = powf(1 - fabsf(direccion.y), 3);
    fondo = Vector3Add(fondo, Vector3Scale((Vector3){0.2f, 0.7f, 0.8f}, 0.25f * horizonte));
    return fondo;
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
    // Me quedo con la esfera más cercana
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

    // Para las coordenadas UV tambien me apoye en Claude para que las texturas quedaran bien en las esferas y en el cielo
    float u = 0.5f + atan2f(-choque.normal.z, choque.normal.x) / (2 * PI);
    float v = 0.5f - asinf(choque.normal.y) / PI;
    int px = (int)(u * (esfera.anchoTextura - 1));
    int py = (int)(v * (esfera.altoTextura - 1));
    int indiceTextura = py * esfera.anchoTextura + px;

    Vector3 colorTextura = {
        esfera.pixelesTextura[indiceTextura].r / 255.0f,
        esfera.pixelesTextura[indiceTextura].g / 255.0f,
        esfera.pixelesTextura[indiceTextura].b / 255.0f};

    if (indice == 0)
    {
        Vector3 colorSol = colorTextura;
        float bordeSol = powf(1 - fmaxf(0, Vector3DotProduct(choque.normal, Vector3Negate(direccion))), 3);
        colorSol = Vector3Add(colorSol, Vector3Scale((Vector3){1, 1, 1}, bordeSol));
        return colorSol;
    }

    // Difusa
    Vector3 dirLuz = Vector3Normalize(Vector3Subtract(posLuz, choque.point));
    float difusa = fmaxf(0, Vector3DotProduct(choque.normal, dirLuz));
    Vector3 color = Vector3Scale(colorTextura, 0.25f + 1.5f * difusa);

    // Especular
    Vector3 reflejo = Vector3Reflect(Vector3Negate(dirLuz), choque.normal);
    float especular = powf(fmaxf(0, Vector3DotProduct(reflejo, Vector3Negate(direccion))), 32);
    color = Vector3Add(color, Vector3Scale((Vector3){1, 1, 1}, 0.6f * especular));

    // Para hacer este borde, Claude me explicó que el punto entre la normal y la cámara
    // es 1 en el centro y 0 en las orillas, entonces solo brilla en el borde
    float borde = powf(1 - fmaxf(0, Vector3DotProduct(choque.normal, Vector3Negate(direccion))), 3);
    color = Vector3Add(color, Vector3Scale((Vector3){0.3f, 0.9f, 1.0f}, borde));

    return color;
}

// Para que la cámara no atraviese los planetas
bool camaraEsfera(Vector3 punto)
{
    for (int i = 0; i < NUM_ESFERAS; i++)
    {
        if (Vector3Distance(punto, esferas[i].centro) < esferas[i].radio + 0.3f)
        {
            return true;
        }
    }
    return false;
}

int main(void)
{
    const int screenWidth = ANCHO * ESCALA;
    const int screenHeight = ALTO * ESCALA;

    InitWindow(screenWidth, screenHeight, "Proyecto 3 Space Travel");
    SetTargetFPS(60);

    float mitad = tanf(60 * DEG2RAD / 2);
    float aspecto = (float)ANCHO / ALTO;

    float anguloHorizontal = 0;
    cam.position = (Vector3){0, 1, 10};
    float velocidad = 4;

    // La textura que voy a pintar
    Image imagenNegro = {
        .data = colores,
        .width = ANCHO,
        .height = ALTO,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8}; // Es el formato que usa LoadImage pero como no puedo usar funciones de dibujo
    Texture2D texturaColores = LoadTextureFromImage(imagenNegro);

    // Cargo las texturas
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

    Image imagenCielo = LoadImage("texturas/milkyway.png");
    if (imagenCielo.data == 0)
    {
        CloseWindow();
        return 1;
    }
    anchoCielo = imagenCielo.width;
    altoCielo = imagenCielo.height;
    pixelesCielo = LoadImageColors(imagenCielo);
    UnloadImage(imagenCielo);

    while (!WindowShouldClose())
    {
        float delta = GetFrameTime();
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            anguloHorizontal -= 1.5f * delta;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            anguloHorizontal += 1.5f * delta;

        Vector3 frente = {sinf(anguloHorizontal), 0, -cosf(anguloHorizontal)};

        Vector3 nuevaPosicion = cam.position;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
            nuevaPosicion = Vector3Add(cam.position, Vector3Scale(frente, velocidad * delta));
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
            nuevaPosicion = Vector3Subtract(cam.position, Vector3Scale(frente, velocidad * delta));
        if (!camaraEsfera(nuevaPosicion))
            cam.position = nuevaPosicion;

        int destinos[4] = {4, 1, 2, 3};
        for (int i = 0; i < 4; i++)
        {
            if (IsKeyPressed(KEY_ONE + i))
            {
                Esfera destino = esferas[destinos[i]];
                cam.position = (Vector3){destino.centro.x, 1, destino.centro.z + destino.radio * 3 + 1};
                anguloHorizontal = 0;
            }
        }
        frente = (Vector3){sinf(anguloHorizontal), 0, -cosf(anguloHorizontal)};

        cameraLook(&cam, Vector3Add(cam.position, frente));

        // En este ciclo se genera un rayo por pixel y me basé en el proyecto base de la clase
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

        // Lo paso a la textura
        UpdateTexture(texturaColores, colores);
        BeginDrawing();
        DrawTextureEx(texturaColores, (Vector2){0, 0}, 0, ESCALA, WHITE);
        EndDrawing();
    }

    for (int i = 0; i < NUM_ESFERAS; i++)
    {
        UnloadImageColors(esferas[i].pixelesTextura);
    }
    UnloadImageColors(pixelesCielo);
    UnloadTexture(texturaColores);
    CloseWindow();
    return 0;
}
