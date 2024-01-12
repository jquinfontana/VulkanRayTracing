# Generar solución Visual Studio:
Requisitos:

* Instalar Vulkan SDK (a partir de la versión 1.2.162.0 hay soporte para ray tracing).

* Instalar Cmake (minima versión requerida: 3.9.6).

**Vulkan SDK**: https://vulkan.lunarg.com/sdk/home

Ejecutar comando `cmake .` en el directorio `vk_raytracing`.

# Agregar un nuevo proyecto: 

* Crear un nuevo directorio que contenga los archivos `main.cpp`, `hello_vulkan.h`, `hello_vulkan.cpp`, un directorio llamado `shaders`
con los shaders correspondientes y el archivo `CMakeList` (el cual se puede reutilizar de otro proyecto sin realizarle cambios). 

* En el archivo `vk_raytracing/CMakeList.txt`, en la sección **Sub examples** agregar la linea:
`add_subdirectory(nombre_directorio)` donde nombre_directorio es el nombre del directorio que hemos creado para nuetsro nuevo proyecto.

* Ejecutar cmake en el directorio `vk_raytracing`.