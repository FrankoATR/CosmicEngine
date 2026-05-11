# WandEngine

## Fecha efectiva

**8 de febrero de 2025**

## Descripción general

En esta etapa la base del proyecto cambia de forma mucho más radical que en las versiones anteriores. El framework deja de apoyarse en Allegro como capa unificada para ventana, render, entrada, audio y recursos, y pasa a reconstruirse alrededor de una pila propia basada en OpenGL y librerías especializadas por responsabilidad.

El cambio visible no es solo una sustitución de APIs: es una reestructuración casi desde cero del engine. Desaparecen del árbol local las cabeceras de `allegro5/`, se eliminan managers antiguos acoplados a esa base y aparecen nuevos módulos organizados por dominio para cámara, entrada, audio, recursos, escenas, cuerpos, luces, objetos y UI. En paralelo se incorpora una ruta de render explícita con shaders, texturas, VAO/VBO, matrices de proyección y soporte inicial para modelos y cámaras 3D.

La consecuencia técnica de esta versión es que el proyecto deja de comportarse como una plantilla dependiente de un framework establecido y pasa a perfilarse como un engine propio, donde la ventana, el contexto gráfico, la gestión del pipeline, la carga de recursos y los subsistemas auxiliares quedan bajo control directo del repositorio.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- sustituir la dependencia monolítica de Allegro por una base propia construida sobre OpenGL y librerías independientes;
- reorganizar el engine en módulos más especializados para render, recursos, entrada, audio, cámara, escenas y objetos;
- abrir el framework a un pipeline gráfico más explícito y extensible, incluyendo soporte visible para 2D, 3D, shaders, texturas, mallas y modelos.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `CMakeLists.txt`, donde la build deja atrás el enlace con Allegro y pasa a declarar una pila explícita con `glad`, `GLFW`, `glm`, `Assimp`, `FMOD`, `ImGui`, `stb_image`, `enet` y `sqlite3`, además del nuevo nombre lógico del proyecto `WandEngine`.
- `include/allegro5/`, que desaparece del árbol local, marcando la salida visible del framework gráfico anterior.
- `include/WandEngine/Managers/`, que deja de concentrarse en archivos planos como `InputManager.hpp` o `SceneManager.hpp` y pasa a organizarse por dominios internos como `Audio/`, `Camera/`, `Body/`, `Event/`, `Input/`, `Light/`, `Object/`, `Resource/`, `Scene/`, `Timer/` y `UI/`.
- `include/WandEngine/Models/`, donde aparecen bloques nuevos para `Camera/`, `Mesh/`, `Model/`, `Shader/`, `Texture/` y `Light/`, mientras se conservan modelos base del runtime como `GameObject`, `GameBodyObject`, `GameScene` y `GameTimer`.
- `include/WandEngine/Models/Shader/Shader.*`, `Texture/GameTexture2D.*`, `Mesh/Mesh.*` y `Model/Model.*`, que materializan la nueva base de render y carga de assets gráficos.
- `include/WandEngine/Managers/Resource/ResourceManager.*`, que se convierte en un punto de unión entre recursos GPU, shaders, texturas, spritesheets y primitivas dibujadas directamente con OpenGL.
- `include/WandEngine/Managers/GameManager.*`, `Input/InputManager.*`, `Camera/CameraManager.*` y `Scene/SceneManager.*`, donde queda la nueva columna vertebral del runtime sin dependencia de Allegro.
- `src/Scenes/MainScene.*` y `src/main.cpp`, donde el juego de ejemplo ya consume la nueva base mediante macros de includes y managers desacoplados del stack anterior.

## Arquitectura o sistemas principales

### Migración del bootstrap gráfico a OpenGL

La sustitución de Allegro se hace visible desde el punto de entrada del runtime. `GameManager` pasa a crear la ventana y el contexto con `GLFW`, inicializa funciones de OpenGL a través de `GLAD`, configura una versión Core Profile 3.3, activa blending por hardware y controla directamente `glViewport`, `glClearColor` y el ciclo de intercambio de buffers.

Con ello el engine deja de depender de una API de alto nivel que encapsulaba render y ventana, y pasa a controlar de forma explícita:

- creación del contexto gráfico;
- versión del pipeline disponible;
- sincronización vertical;
- relación entre update fijo y render desacoplado;
- redimensionamiento del framebuffer;
- mantenimiento de aspect ratio 16:9 dentro del viewport.

Esa decisión es importante porque desplaza el motor desde un modelo consumido hacia un modelo implementado: la capa gráfica ya no viene resuelta por un framework externo, sino que empieza a quedar definida por clases del propio proyecto.

### Pipeline de render propio y explícito

La incorporación de `Shader`, `GameTexture2D` y el nuevo `ResourceManager` marca el cambio técnico más profundo de la versión. En lugar de limitarse a cargar y dibujar bitmaps mediante una API cerrada, el engine ahora expone directamente los elementos centrales del pipeline moderno:

- compilación y enlace de vertex, fragment y geometry shaders;
- subida de uniforms para matrices, vectores, colores y samplers;
- administración de texturas 2D con `glTexImage2D` y parámetros de wrapping y filtering;
- generación y reutilización de `VAO` y `VBO` para sprites y primitivas;
- proyecciones y transformaciones mediante matrices `glm::mat4`.

Esto trae una ventaja estructural importante frente al enfoque anterior: el render ya no está atado a la forma en que Allegro decide representar sprites o primitivas. A partir de aquí, el proyecto puede definir su propio contrato visual, sus propios shaders, su propia composición de recursos GPU y su propia evolución hacia efectos, batching, iluminación o postproceso sin cambiar de framework base.

### Recursos desacoplados por tipo y por subsistema

La migración también cambia la forma en que se entienden los recursos del engine. `ResourceManager` ya no actúa solo como contenedor de imágenes o fuentes, sino como un coordinador de recursos gráficos nativos del pipeline:

- texturas cargadas como `GameTexture2D`;
- spritesheets con cálculo explícito de UV;
- shaders registrados por clave;
- geometría base asociada a VAO;
- primitivas dibujadas con `GL_LINES`, `GL_LINE_LOOP` u otras rutas de OpenGL.

Desde el punto de vista de diseño, esto es una mejora clara frente al enfoque anterior porque separa el concepto de recurso de la librería que lo consume. La textura deja de ser “una imagen de Allegro” y pasa a ser un recurso propio del engine con ciclo de vida, formato y binding controlados por el runtime.

### Apertura explícita a 3D

La nueva estructura introduce piezas que no existían en la etapa Allegro: `Camera3D`, `Mesh`, `Model`, `Light`, `Shader` y `Texture`. Además, `CameraManager` ya distingue entre `CAMERA_2D` y `CAMERA_3D`, devuelve tanto view matrix como projection matrix y redirige entrada de mouse y teclado hacia una cámara tridimensional cuando corresponde.

También queda visible la incorporación de `Assimp` dentro de la build y de clases `Mesh` y `Model` con vértices, normales, coordenadas UV, tangentes, bitangentes e información para huesos. Aunque el juego de ejemplo visible siga siendo mayormente 2D, la infraestructura del engine ya no está pensada solo para sprites: queda preparada para consumir modelos, mallas y cámaras de escena más complejas.

Esta es una diferencia importante a favor de OpenGL y de una pila modular. En lugar de depender de lo que un framework 2D permita o no permita extender, el motor pasa a apoyarse en una API gráfica de propósito general que puede sostener tanto el modo 2D actual como un crecimiento gradual hacia 3D real dentro del mismo proyecto.

### Librerías independientes en lugar de framework monolítico

Otro cambio estructural fuerte es la sustitución de una sola dependencia grande por varias librerías especializadas:

- `GLFW` para creación de ventana, contexto y eventos básicos;
- `GLAD` para carga de funciones OpenGL;
- `glm` para álgebra lineal, matrices y vectores;
- `stb_image` para carga de imágenes;
- `Assimp` para importación de modelos;
- `FMOD` para audio y reproducción multimedia;
- `ImGui` para tooling e interfaz de depuración;
- `enet` y `sqlite3` como piezas auxiliares visibles en la configuración del build.

Técnicamente, esta decisión tiene varias ventajas frente al uso de Allegro como capa integral:

- reduce el acoplamiento entre subsistemas que evolucionan a ritmos distintos;
- permite sustituir o mejorar una parte concreta del stack sin reescribir todo el engine;
- acerca el proyecto a prácticas comunes de motores propios y pipelines gráficos modernos;
- deja más control sobre rendimiento, formato de recursos, render pass, input y audio;
- evita que el modelo mental del framework quede restringido por una sola biblioteca generalista.

En otras palabras, el proyecto ya no delega la arquitectura principal a una librería “todo en uno”, sino que compone su propio runtime a partir de piezas especializadas que puede conectar y adaptar según sus necesidades.

### Reorganización modular del engine

La reestructuración de carpetas también es relevante desde el punto de vista del diseño. Los managers dejan de existir como una capa plana de archivos heredados y pasan a agruparse por dominio interno. Ese cambio hace visible una intención de arquitectura más estable:

- audio separado entre música y sonido;
- input separado de cámara;
- escena separada de objetos y cuerpos;
- recursos gráficos separados de modelos de render;
- UI separada del resto del runtime.

Además, el propio código del juego consume estos módulos mediante macros de include y aliases de managers, lo que sugiere una transición hacia una API pública más controlada del engine.

### Escena de ejemplo adaptada al nuevo runtime

`MainScene` ya no se apoya en Allegro para cargar sprites, leer entrada o dibujar overlays. La escena actual usa:

- texturas y spritesheets cargados por el nuevo `ResourceManager`;
- entrada de teclado y mouse gestionada por `GLFW` a través de `InputManager`;
- panel de control y depuración montado con `ImGui`;
- audio reproducido por los nuevos managers basados en `FMOD`;
- cámara 2D con matrices ortográficas derivadas del nuevo `CameraManager`.

Aunque todavía se conservan elementos del gameplay anterior, la forma en que la escena los consume ya pertenece claramente a otro motor. Esa continuidad de entidades y escenas sobre una base técnica distinta es precisamente lo que hace visible que esta versión reutiliza módulos conceptuales previos, pero los reconstruye sobre un stack propio.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- el engine ya controla directamente el contexto OpenGL, el ciclo fijo de update, el render desacoplado y el viewport desde `GameManager`;
- los recursos gráficos, la cámara, la entrada, el audio y la UI ya se articulan como subsistemas independientes que la escena consume sin depender de la API anterior de Allegro.

Con ello dejé una base mucho más adecuada para evolucionar hacia un engine propio. La ventaja principal de esta etapa no es únicamente que el proyecto use OpenGL, sino que el control del pipeline, de la composición del stack y de la arquitectura del runtime pasa a estar en el repositorio y no en un framework externo ya definido.

## Dependencias externas visibles

En esta etapa el cambio de dependencias es uno de los rasgos más visibles de la versión. Sale del árbol local la dependencia de `allegro5` y pasan a quedar integradas o declaradas en build varias librerías independientes:

- `GLFW`;
- `GLAD`;
- `glm`;
- `Assimp`;
- `FMOD`;
- `ImGui`;
- `stb_image`;
- `enet`;
- `sqlite3`.

El valor técnico de esta transición es que cada una cubre un problema concreto del engine sin imponer una capa única para todo el runtime. Eso vuelve más clara la frontera entre ventana, render, matemáticas, audio, importación de modelos, tooling y persistencia.

## Resumen técnico de la versión

La etapa efectiva al **8 de febrero de 2025** queda delimitada por estos movimientos:

- reemplacé la base Allegro del proyecto por una arquitectura nueva apoyada en OpenGL y librerías independientes;
- eliminé del árbol local la dependencia visible de `allegro5` y reorganicé el engine en módulos por dominio;
- migré el arranque del runtime a `GLFW` + `GLAD`, con control directo de contexto, viewport, frame pacing y swap buffers;
- incorporé un pipeline propio de render con shaders, texturas, VAO/VBO, matrices de proyección y dibujado explícito de sprites y primitivas;
- abrí la estructura del engine hacia 3D mediante cámaras dedicadas, modelos, mallas, luces y soporte de importación con `Assimp`;
- sustituí los subsistemas integrados del framework anterior por librerías especializadas como `FMOD`, `ImGui`, `glm` y `stb_image`.

Con esta etapa dejé el proyecto en un punto de inflexión técnico: ya no se trata de una plantilla sobre un framework ajeno, sino del arranque visible de un engine propio que reutiliza conceptos previos, pero redefine desde su base la ventana, el render, la carga de recursos, la cámara, la entrada y el audio.