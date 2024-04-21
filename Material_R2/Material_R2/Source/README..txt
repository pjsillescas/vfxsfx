MATERIAL PEC2
Anotaciones a estos códigos

- Para poder probar cada reto, es necesario compilar el fichero glad.c junto con el fichero cpp del reto.
Este fichero glad.c está en : Libs\glad\src\glad.c
Añadidlo al proyecto que compiléis siempre.

- En la carpeta EXE está la carpeta Assets. Allí está todo lo que necesitan estos retos para ejecutarse: imágenes, formas 3D y shaders. Deber estar allí donde asignéis como directorio/carpeta de trabajo del ejecutable que generéis.

- Del reto 05 al 09, es necesario añadir a compilar el fichero shader.cpp junto con el fichero cpp del reto.

- Del reto 09 al reto 15, retos ya del ejemplo de crear el efecto de agua, cada uno tiene su propia carpeta con los ficheros necesarios. Esto es así dado que algunos tienen pequeñas modificaciones y para tenerlos mejor clasificados.

- Por otra parte, el tema de la configuración de este proyecto, para que no andeis dando vueltas, os he dejado en el drive de estos archivos el fichero OpenGLGlad - Visual Studio Project.zip con un proyecto listo y configurado para ejecutar los ejercicios y hacer vuestras pruebas (SDL / Glad ).
Tened en cuenta que hay que poner los DLL de x86 o de x64 de la carpeta Libs\VisualC\SDL2\lib\ correspondientes en la raiz del proyecto, dependiendo si usais Win32 o x64 al compilar.

RAFEL PEREZ - Abril 2023


