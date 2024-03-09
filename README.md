# Ray Tracing en Vulkan

<img src="https://github.com/jquinfontana/VulkanRayTracing/blob/main/documentos/imagenes%20path%20tracing/dragonPortada.jpg" width="49%"></img> <img src="https://github.com/jquinfontana/VulkanRayTracing/blob/main/documentos/imagenes%20path%20tracing/waterPortada.jpg" width="49%"></img>

# Generar solución Visual Studio:
Requisitos:

* Instalar Vulkan SDK (a partir de la versión 1.2.162.0 hay soporte para ray tracing).

* Instalar Cmake (minima versión requerida: 3.9.6).

* Instalar Visual Studio 2022 

**Vulkan SDK**: https://vulkan.lunarg.com/sdk/home

**Paso 1**: Copiar el contenido del repositorio [nvpro core](https://github.com/nvpro-samples/nvpro_core) en un carpeta `nvpro_core` dentro de nuestro repositorio.

La estructura deberia ser la siguiente:

<img src="https://github.com/jquinfontana/VulkanRayTracing/blob/main/documentos/imagenes%20path%20tracing/estructura%20repo.png" width="20%"></img>

**Paso 2**: Ejecutar comando `cmake .` en el directorio `vk_raytracing`. Para esto se recomienda agregar Cmake al path en el momento de la instalación.

La estructura de la solución deberia ser algo parecido a esto:

<img src="https://github.com/jquinfontana/VulkanRayTracing/blob/main/documentos/imagenes%20path%20tracing/solucionvisual.png" width="30%"></img>

# Agregar un nuevo proyecto: 

* Crear un nuevo directorio que contenga los archivos `main.cpp`, `hello_vulkan.h`, `hello_vulkan.cpp`, un directorio llamado `shaders`
con los shaders correspondientes y el archivo `CMakeList` (el cual se puede reutilizar de otro proyecto sin realizarle cambios). 

* En el archivo `vk_raytracing/CMakeList.txt`, en la sección **Sub examples** agregar la linea:
`add_subdirectory(nombre_directorio)` donde `nombre_directorio` es el nombre del directorio que hemos creado para nuetsro nuevo proyecto.

* Ejecutar cmake en el directorio `vk_raytracing`.
