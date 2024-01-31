# Ray Tracing en Vulkan

<img src="https://github.com/jquinfontana/VulkanRayTracing/blob/main/documentos/imagenes%20path%20tracing/dragonPortada.jpg" width="49%"></img> <img src="https://github.com/jquinfontana/VulkanRayTracing/blob/main/documentos/imagenes%20path%20tracing/waterPortada.jpg" width="49%"></img>

# Generar solución Visual Studio:
Requisitos:

* Instalar Vulkan SDK (a partir de la versión 1.2.162.0 hay soporte para ray tracing).

* Instalar Cmake (minima versión requerida: 3.9.6).

**Vulkan SDK**: https://vulkan.lunarg.com/sdk/home

**Cmake**: https://cmake.org

Ejecutar comando `cmake .` en el directorio `vk_raytracing`. Para esto se recomienda agregar Cmake al path en el momento de la instalación.

# Agregar un nuevo proyecto: 

* Crear un nuevo directorio que contenga los archivos `main.cpp`, `hello_vulkan.h`, `hello_vulkan.cpp`, un directorio llamado `shaders`
con los shaders correspondientes y el archivo `CMakeList` (el cual se puede reutilizar de otro proyecto sin realizarle cambios). 

* En el archivo `vk_raytracing/CMakeList.txt`, en la sección **Sub examples** agregar la linea:
`add_subdirectory(nombre_directorio)` donde `nombre_directorio` es el nombre del directorio que hemos creado para nuetsro nuevo proyecto.

* Ejecutar cmake en el directorio `vk_raytracing`.

# Path tracer

Se implementó un path tracer a modo de referencia/demo basado en el libro [Ray Tracing in One Weekend](https://raytracing.github.io/), el cual implementa efectos como: defocus blur, gamma correction, materiales glossy, fresnel effect, gaussian filter antialiasing, etc.
<img src="https://github.com/jquinfontana/VulkanRayTracing/blob/main/documentos/imagenes%20path%20tracing/dragon3.jpg" width="49%"></img> <img src="https://github.com/jquinfontana/VulkanRayTracing/blob/main/documentos/imagenes%20path%20tracing/sponza2.jpg" width="49%"></img> <img src="https://github.com/jquinfontana/VulkanRayTracing/blob/main/documentos/imagenes%20path%20tracing/catedral.jpg" width="49%"></img> <img src="https://github.com/jquinfontana/VulkanRayTracing/blob/main/documentos/imagenes%20path%20tracing/dragon5.jpg" width="49%"></img>
