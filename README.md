# Cambios Técnicos del Framework desde el README anterior

## 1. Ajustes de la API 3D de `ResourceManager`

Se corrigió la superficie pública del renderer para que el modo 3D exponga realmente las primitivas que ya estaban siendo utilizadas o implementadas internamente.

Cambios incluidos:
- Se añadieron en el header las declaraciones públicas faltantes de primitivas 3D.
- Se formalizó `RenderLine(...)`, `RenderTriangle(...)`, `RenderRectangle(glm::vec3, ...)` y `RenderParallelepipedLines(...)` dentro de la API 3D.
- Se evitó que el header 3D quedara desfasado respecto a las implementaciones reales del `.cpp`.

Impacto técnico:
- El build vuelve a ser coherente entre interfaz pública e implementación.
- Se eliminan errores de compilación derivados de símbolos 3D presentes en `.cpp` pero no declarados en `.hpp`.

Archivos relevantes:
- `include/CosmicEngine/managers/resource/resource_manager.hpp`
- `include/CosmicEngine/managers/resource/resource_manager.cpp`

## 2. Compatibilidad de UI 2D dentro del build 3D

Se añadió soporte explícito para seguir dibujando primitivas 2D de interfaz aun cuando el juego compile en modo 3D.

Cambios incluidos:
- Se agregó el overload `RenderRectangle(glm::vec2, ...)` también en la rama `GAME_3D_CONFIGURATION`.
- Esa ruta usa el shader 2D y la proyección de UI en lugar de mezclarla con la tubería 3D.

Impacto técnico:
- Los widgets y elementos UI que dependen de rectángulos 2D siguen funcionando en builds 3D.
- Se evita romper componentes de interfaz por asumir que modo 3D implica API exclusivamente 3D.

Archivos relevantes:
- `include/CosmicEngine/managers/resource/resource_manager.hpp`
- `include/CosmicEngine/managers/resource/resource_manager.cpp`

## 3. Corrección de semántica espacial en `RenderParallelepipedLines(...)`

Se corrigió una inconsistencia crítica entre cómo el framework dibujaba wireframes 3D y cómo el resto del motor interpreta los volúmenes espaciales.

Problema corregido:
- Colisiones, cuerpos y áreas espaciales usan `position` como esquina mínima del volumen.
- El wireframe 3D estaba construyendo la transformación como si `position` fuese el centro.

Cambio aplicado:
- La matriz modelo ahora calcula el centro a partir de `position + size * 0.5f` antes de escalar el cubo base.

Impacto técnico:
- El volumen visual del renderer coincide con el volumen lógico usado por grid, octree y cuerpos.
- El debug 3D deja de mostrar desplazamientos aparentes que en realidad provenían de una convención espacial distinta en rendering.

Archivos relevantes:
- `include/CosmicEngine/managers/resource/resource_manager.cpp`

## 4. Ajuste del debug 3D en `Body`

Se cambió la representación visual de debug de cuerpos 3D para que dibuje el volumen completo y no una sola cara.

Cambios incluidos:
- `Body::draw()` en 3D dejó de usar un rectángulo plano.
- Ahora utiliza `RenderParallelepipedLines(...)` con `ViewType::Projection`.

Impacto técnico:
- El debug de cuerpos pasa a representar la caja de colisión completa.
- Se elimina el artefacto visual del “cuadrado rojo” que no representaba correctamente el cuerpo 3D.

Archivos relevantes:
- `include/CosmicEngine/models/body/body.cpp`

## 5. Ajuste de proyección en `CameraManager`

Se amplió el plano lejano de la cámara en perspectiva para mejorar la visualización de escenas 3D más grandes y volúmenes de depuración extensos.

Cambios incluidos:
- El far plane pasó de `100.0f` a `300.0f`.

Impacto técnico:
- Se reduce el clipping prematuro al inspeccionar áreas espaciales más profundas.
- El debug de colisiones 3D puede visualizarse completo desde posiciones de cámara más alejadas.

Archivos relevantes:
- `include/CosmicEngine/managers/camera/camera_manager.cpp`

## 6. Ajuste de build para fuentes exclusivas por modo

Se actualizó CMake para compilar archivos específicos según la configuración activa del juego.

Cambios incluidos:
- La lista de fuentes ahora agrega el archivo de prueba 3D solo cuando `GAME_MODE_CONFIGURATION` está en modo 3D.
- La separación de fuentes por modo queda explícita en el build.

Impacto técnico:
- Se evita compilar fuentes de prueba de un modo dentro del otro de forma innecesaria.
- La configuración del proyecto queda más estricta respecto a aislamiento entre ramas 2D y 3D.

Archivo relevante:
- `CMakeLists.txt`

## Resumen

Desde el README anterior, los cambios reutilizables del framework se concentraron en tres áreas:
- saneamiento de la API pública de rendering 3D
- consistencia espacial entre debug visual y volúmenes reales
- compatibilidad operativa del build 3D con UI y depuración

Este documento es incremental y solo cubre ese tramo nuevo de trabajo.