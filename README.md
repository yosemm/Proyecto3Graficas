# Proyecto 3 de Gráficas por Computadora

## Video explicando:

https://youtu.be/rm3UTYwSMDI

## Programa funcionando:

https://github.com/user-attachments/assets/16dc7b9e-a853-4973-9992-126830dd44ae

## Controles:

- W/S o tecla Arriba/Abajo: Se mueve hacia adelante o hacia atras
- A/D o tecla Izquierda/Derecha: Girar
- Para hacer viaje rapido se pueden presionar los numeros:  1. Luna 2. Venus 3. Tierra 4. Marte
  
## Comando para compilar el proyecto:
```bash
gcc main.c -o main -O2 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

Luego para ejecutar:
```bash
./main
```

## Uso de Raylib:
Mis usos de Raylib fueron únicamente:
- Usé Raylib para levantar la ventana, recibir el input del teclado, usar los tipos y funciones de vectores y cargar las imágenes para usarlas como texturas. 
- El código calcula el color de cada píxel y lo guarda en un array. Finalmente, usé UpdateTexture y DrawTextureEx para mostrar ese array de colores en la ventana.
  
## Referencias: 

- Texturas para el Sol, Tierra, Marte, Venus las encontré en https://planet-texture-maps.fandom.com/wiki/ 
- Texturas para las estrellas del skybox y la luna, las encontré en https://www.solarsystemscope.com/textures/
- Para este proyecto me apoyé en Claude para las partes que marque en el código usando comentarios, como el brillo en los bordes de los planetas y ajustar las coordenadas UV.
