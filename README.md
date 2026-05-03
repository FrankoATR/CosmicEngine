# Cambios Técnicos Recientes del Framework

## Alcance del documento

La documentación se concentra en los cambios funcionales incorporados a nivel de framework: la reestructuración del sistema de build con CMake, la forma en que el motor descubre y configura proyectos, la propagación de variables de proyecto hacia el runtime, la evolución del `LightManager`, los ajustes del pipeline de renderizado 3D y la mejora del sistema de importación de modelos y materiales.

## 1. Reestructuración del sistema de build y selección de proyectos

### 1.1 Eliminación de la dependencia estructural de `src/`

Se retiró el supuesto de que la carpeta `src/` debía contener la escena principal y el punto de entrada del programa. En su lugar, el build fue reorganizado para que cada proyecto bajo `projects/<nombre>` se convirtiera en la unidad principal de configuración y compilación.

Este cambio resolvió uno de los problemas estructurales más importantes del framework: la necesidad de mantener una carpeta fuente central que mezclaba escenas, lógica de arranque y componentes del engine. A partir de esta reestructuración, el framework puede compilar distintos proyectos sin depender de una escena o conjunto de clases globales “por defecto”.

### 1.2 Incorporación de `project.cmake` por proyecto

El archivo principal de build ahora descubre dinámicamente los directorios ubicados bajo `projects/` y exige que cada proyecto válido exponga su configuración mediante un archivo `project.cmake` propio.

Este mecanismo trasladó la autoridad de configuración desde el `CMakeLists.txt` raíz hacia cada proyecto individual. Con ello se resolvieron varias limitaciones previas:

- Se eliminó la necesidad de editar el build raíz para cambiar de escena inicial o de configuración de arranque.
- Se permitió que cada proyecto declare su propio modo de juego (`GAME_2D_CONFIGURATION` o `GAME_3D_CONFIGURATION`).
- Se habilitó que cada proyecto defina sus propias fuentes, resolución base, fullscreen inicial, VSync inicial, título de ventana, ícono y nombre del ejecutable.

### 1.3 Validación explícita de variables mínimas de proyecto

El `CMakeLists.txt` principal fue reforzado para validar que cada `project.cmake` defina, como mínimo, el modo del juego, la primera escena y la clase asociada a esa escena. Si estos datos no existen, el proceso de configuración falla inmediatamente.

Esta decisión solucionó una debilidad importante del flujo anterior: antes era posible mantener configuraciones implícitas o incompletas que derivaban en builds ambiguos o en fallos tardíos durante la compilación o ejecución.

### 1.4 Presets por plataforma y por proyecto

`CMakePresets.json` fue ampliado para exponer presets concretos por sistema operativo, proyecto y tipo de compilación. El esquema actual define presets base para Windows y Linux, y a partir de ellos genera variantes Debug y Release por proyecto.

Este cambio resolvió varios problemas operativos:

- Evitó la reconfiguración manual del cache de CMake al cambiar de proyecto.
- Formalizó la selección del proyecto activo mediante la variable cache `COSMIC_PROJECT`.
- Preparó el repositorio para soportar builds equivalentes en Windows y Linux sin duplicar lógica de configuración.

### 1.5 Salidas separadas por proyecto y plataforma

El sistema de salida fue reorganizado para que cada ejecutable y sus assets se ubiquen bajo `bin/<proyecto>/<plataforma>/<configuración>/`.

Además, se incorporó una tarea de copiado de assets por proyecto que primero elimina el contenido previo del directorio de assets del build y luego copia únicamente el árbol del proyecto activo. Con ello se corrigió el problema de retener recursos obsoletos de ejecuciones previas dentro del directorio de salida.

### 1.6 Preparación de empaquetado Linux

El build raíz ahora contiene rutas y scripts auxiliares para empaquetar dependencias compartidas en Linux y generar un launcher específico para ese entorno. Aunque la sesión reciente se ejecutó en Windows, la estructura del build ya quedó adaptada para una salida multiplataforma real.

Este ajuste resolvió la limitación previa en la que el framework estaba diseñado casi exclusivamente para entorno Windows y requería trabajo manual para correr fuera de ese sistema.

## 2. Propagación de variables de proyecto hacia el runtime

### 2.1 Bootstrap genérico del ejecutable principal

El archivo `main.cpp` fue reemplazado por un punto de entrada genérico que ya no depende de incluir una escena fija del repositorio. El nuevo bootstrap utiliza dos archivos generados por CMake:

- `cosmic_project_entry.hpp`
- `cosmic_project_config.hpp`

Con este cambio, el ejecutable principal deja de estar acoplado a un proyecto concreto y pasa a inicializar el framework en función de la configuración declarada por el proyecto activo.

### 2.2 Generación de entrada de primera escena

`cosmic_project_entry.hpp.in` genera una función de fábrica que incluye el header de la escena inicial definida en el `project.cmake` del proyecto seleccionado y devuelve una instancia de esa escena.

Este cambio solucionó el acoplamiento previo entre el `main` del engine y una escena concreta. Ahora la escena inicial es un dato de configuración del proyecto, no una decisión codificada manualmente dentro del ejecutable.

### 2.3 Generación de configuración de ventana y arranque

`cosmic_project_config.hpp.in` genera constantes compiladas para el nombre del proyecto, el título de ventana, el ícono, la resolución inicial, la resolución base de render, el fullscreen inicial y el estado inicial de VSync.

Esto resolvió otro problema estructural relevante: antes esas variables estaban repartidas entre definiciones generales y lógica hardcodeada. Ahora el runtime las recibe como un bloque consistente derivado del proyecto activo.

## 3. Evolución del `GameManager` y del arranque del runtime

### 3.1 Introducción de `GameManagerInitParams`

`GameManager` fue adaptado para inicializarse mediante una estructura de parámetros que agrupa el título de ventana, la ruta del ícono, la resolución de la ventana, la resolución base y las banderas iniciales de fullscreen y VSync.

Este rediseño solucionó el problema de tener inicializaciones dispersas o dependientes de constantes globales. El arranque del runtime pasó a ser explícito, declarativo y alineado con la configuración del proyecto activo.

### 3.2 Aplicación temprana de título, ícono, fullscreen y VSync

Durante `GameManager::init`, el runtime ahora aplica el título de la ventana, el ícono configurado, el estado inicial de VSync y la transición a fullscreen cuando corresponde.

Este cambio resolvió una carencia visible del framework anterior: aunque algunos valores podían cambiarse posteriormente, no existía un mecanismo claro para que el proyecto arrancara directamente con sus preferencias declaradas.

### 3.3 Mantención de aspect ratio base como contrato de proyecto

La configuración de aspecto base quedó enlazada con la resolución virtual definida por cada proyecto. Esto permite que la cámara y el viewport mantengan un comportamiento consistente con la intención del proyecto activo, en lugar de depender de un único valor compartido por todo el repositorio.

## 4. Revisión y ampliación del `LightManager`

### 4.1 Separación formal entre luces estáticas y dinámicas

El `LightManager` fue ampliado para mantener registros diferenciados de luces estáticas y dinámicas, así como un cache específico para las luces estáticas.

Este rediseño no cambia la forma en que el shader calcula la luz por fragmento, pero sí modifica la forma en que el motor selecciona qué luces subir al shader en cada frame. La clasificación entre luces estáticas y dinámicas ahora expresa intención de uso y estrategia de actualización, no un modelo de sombreado distinto.

### 4.2 Presupuesto explícito de luces activas en GPU

Se introdujo el concepto de `maxActivePointLights`, con un valor por defecto alineado al presupuesto máximo que los shaders del framework contemplan actualmente.

Este cambio resolvió un problema de escalabilidad: el framework ya no intenta mandar indiscriminadamente todas las luces al shader, sino que controla cuántas son activas por frame según el presupuesto disponible.

### 4.3 Selección de luces activas por prioridad y cercanía a cámara

La lógica de `LightManager::update` fue reformulada para ordenar las luces visibles según una prioridad basada en distancia a la cámara. Luego combina luces estáticas cacheadas y luces dinámicas activas hasta completar el presupuesto máximo.

Esto resolvió el problema inmediato de soportar escenas con más luces de las que el shader puede recibir simultáneamente. Sin embargo, también introdujo una consecuencia visual conocida y ya observada durante las pruebas: la iluminación activa cambia al mover la cámara, porque la selección se hace respecto al observador y no respecto al objeto renderizado.

### 4.4 Introducción de histéresis para suavizar el recambio de luces

Se añadió un mecanismo de histéresis mediante `lastActivePointLightIds` y un umbral de distancia adicional para las luces que ya estaban activas en el frame anterior.

Esta medida no elimina el popping por completo, pero sí atenúa cambios excesivamente bruscos cuando dos luces compiten por entrar o salir del conjunto activo.

### 4.5 Cache y refresco condicionado de luces estáticas

Las luces estáticas ahora se reordenan y refrescan solo cuando el cache queda marcado como sucio o cuando la cámara se desplaza más allá de un umbral mínimo.

Este cambio resolvió una ineficiencia anterior: evitar trabajo redundante cada frame cuando la parte estática del entorno no ha cambiado de manera significativa.

### 4.6 Registro de shaders receptores de iluminación

El `ResourceManager` quedó preparado para registrar el shader `COSMIC_3DModel_Lit` en el `LightManager`, de forma que el manager pueda empujar uniformes globales y luces activas a los shaders relevantes.

Este ajuste resolvió la desconexión previa entre la administración centralizada de luces y los shaders lit del render 3D.

## 5. Cambios en `Light` como modelo de datos

La clase `Light` fue adaptada para reflejar con mayor claridad su movilidad, sus parámetros de atenuación, su shininess y sus contribuciones ambient, diffuse y specular tanto en 2D como en 3D.

Esto fortaleció la coherencia entre los datos mantenidos por cada luz, la selección que realiza el `LightManager` y los uniformes que reciben los shaders de iluminación.

## 6. Ajustes al `ResourceManager` y al pipeline 3D

### 6.1 Formalización de shaders built-in para modelos lit y unlit

El `ResourceManager` ahora carga explícitamente los shaders `COSMIC_3DModel_Lit` y `COSMIC_3DModel_Unlit`, y registra el shader lit en el `LightManager` para que reciba las luces activas.

Este cambio consolidó una ruta de render 3D más clara y separó correctamente el render de materiales afectados por luz del render puramente emissive o de apoyo visual.

### 6.2 Corrección del pivote de rotación para primitivas y modelos 3D

Las funciones de renderizado 3D del `ResourceManager` fueron ajustadas para usar explícitamente la posición del objeto como pivote de transformación al construir la matriz de modelo.

Esta corrección solucionó un problema visual importante del pipeline 3D: ciertas primitivas y volúmenes podían rotar desplazados o alrededor de un origen incorrecto debido a la convención de geometría centrada de las primitivas 3D del engine.

### 6.3 Consolidación de renderizado de paralelepípedos texturizados y modelados 3D

Las rutas `RenderParallelepipedTextureLit`, `RenderParallelepipedTextureUnlit`, `RenderParallelepipedTextureAtlasLit`, `RenderParallelepipedTextureAtlasUnlit`, `Render3DModelLit` y `Render3DModelUnlit` quedaron alineadas bajo un flujo homogéneo de matrices de vista, proyección y modelo.

Con ello se resolvieron inconsistencias entre primitivas internas del engine y modelos importados externamente.

## 7. Revisión del shader 3D y comportamiento material

El shader lit del engine se mantuvo en un modelo clásico tipo Phong/Blinn-Phong, pero fue verificado y puesto en uso consistente con el `LightManager` para que utilice:

- `dirLight` como contribución ambiental/direccional global.
- `pointLights[]` como conjunto activo de luces puntuales.
- `material.diffuse` y `material.specular` como textura y mapa especular cuando están presentes.
- `viewPos` para el cálculo del componente especular.

Esto no convirtió el engine en un renderer PBR, pero sí resolvió la ruta funcional de materiales lit para los modelos y volúmenes 3D incorporados en la escena.

## 8. Mejora de importación de modelos y texturas

### 8.1 Normalización espacial de modelos importados

La clase `Model` fue ampliada para calcular bounds globales del asset importado, recentrarlo en el origen y normalizar su escala general.

Este ajuste solucionó un problema funcional del importador: un modelo podía existir y dibujarse correctamente, pero quedar visualmente desplazado respecto a su pivote o en una escala difícil de controlar desde la lógica de escena.

### 8.2 Soporte para texturas glTF/GLB embebidas y `BASE_COLOR`

La importación de materiales dejó de depender exclusivamente de texturas externas tipo `DIFFUSE` y fue extendida para reconocer también texturas `BASE_COLOR` y texturas embebidas dentro del propio asset importado por Assimp.

Este cambio resolvió el problema por el cual ciertos modelos importados podían aparecer sin su textura correcta o excesivamente oscuros pese a que el asset sí contenía información de material.

### 8.3 Ajustes en `Mesh::draw`

El flujo de binding de texturas en `Mesh` fue alineado con las banderas `hasDiffuseTexture` y `hasSpecularTexture`, de forma que el shader pueda decidir correctamente cuándo usar textura real y cuándo caer a un color base.

## 9. Efecto global de los cambios

En conjunto, los cambios recientes produjeron una transición importante del framework hacia una arquitectura donde:

- el proyecto activo se descubre y configura desde CMake,
- el runtime recibe su configuración desde archivos generados automáticamente,
- el build soporta múltiples proyectos y plataformas de forma explícita,
- el sistema de iluminación puede trabajar con más luces de las que el shader soporta simultáneamente,
- el pipeline 3D maneja de forma más consistente pivotes, modelos importados y materiales,
- y la base del motor quedó menos acoplada a configuraciones hardcodeadas y más orientada a proyectos declarativos.

## 10. Observaciones técnicas pendientes

Aunque la base quedó considerablemente más robusta, todavía existen decisiones de arquitectura que convendrá revisar en iteraciones posteriores:

### 10.1 Selección de luces activas basada en cámara

La solución actual del `LightManager` prioriza luces por distancia a cámara. Esto resuelve el presupuesto de GPU con bajo costo, pero genera transiciones de iluminación perceptibles cuando el observador se mueve. En una evolución posterior, sería recomendable evaluar una selección de luces por objeto renderizado o por partición espacial.

### 10.2 Sombreado de materiales aún no PBR

El soporte de materiales de modelos importados quedó funcional para diffuse/base color y especular simple, pero el pipeline todavía no interpreta un modelo glTF PBR completo. El comportamiento actual es suficiente para el framework, aunque no representa todavía una solución físicamente más precisa.

### 10.3 Limpieza final de assets legacy en raíz

El árbol de trabajo actual refleja la eliminación o migración de recursos previos del directorio legacy de `assets/`. Esta transición refuerza el nuevo modelo por proyecto, pero conviene consolidarla con el próximo commit para dejar completamente estabilizada la separación entre assets globales históricos y assets específicos por proyecto.
