#Generar proyecto:
ejecutar comando "cmake ." en el directorio vk_raytracing.

#Agregar un nuevo proyecto:
Crear un nuevo directorio que contenga los archivos main.cpp, hello_vulkan.h, hello_vulkan.cpp, un directorio llamado "shaders"
con los shaders correspondientes y el archivo CMakeList (el cual se puede reutilizar de otro proyecto sin realizarle cambios). 

En el archivo:
vk_raytracing/CMakeList.txt

En la sección "Sub examples" agregar la linea:
"add_subdirectory(nombre_directorio_nuevo_proyecto)"

y ejecutar cmake en el directorio vk_raytracing.