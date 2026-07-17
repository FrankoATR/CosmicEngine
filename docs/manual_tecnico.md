# Manual Técnico — CosmicEngine

> **Versión del documento:** 1.0  
> **Plataformas soportadas:** Windows 11 (MSYS2 UCRT64 / MinGW-w64), Linux (GCC)  
> **Estándar de compilación:** C++20  
> **Renderizado:** OpenGL 3.3 Core Profile

---

## 1. Introducción

CosmicEngine es un framework de videojuegos en C++ con OpenGL diseñado para facilitar el desarrollo de proyectos 2D y 3D. El motor está organizado como una biblioteca de cabeceras y fuentes que el sistema de construcción (CMake) integra automáticamente con el código del proyecto del desarrollador.

El motor no es un ejecutable independiente: actúa como capa de abstracción sobre OpenGL, GLFW, FreeType, ENet y miniaudio, exponiendo una API en C++ orientada a objetos que el desarrollador extiende mediante herencia y callbacks.

### 1.1 Características principales

| Área | Capacidad |
|---|---|
| Renderizado 2D | Sprites, spritesheets, capas de dibujado ordenadas, iluminación dinámica (hasta 32 luces de punto), canal alfa independiente |
| Renderizado 3D | Mallas importadas desde archivos .obj/.fbx/.gltf (Assimp), iluminación Phong (hasta 8 luces de punto) |
| Texto | FreeType con soporte UTF-8 completo (Latin Básico + Suplemento Latin-1 U+00A0–U+00FF) |
| Entrada | Teclado, ratón y gamepad; sistema de acciones con bindings configurables; navegación UI por teclado/gamepad |
| Audio | miniaudio con múltiples voces, volumen maestro/música/SFX independientes, audio 2D y 3D |
| Red | UDP confiable sobre ENet; roles host/cliente; mensajes JSON; hasta 32 clientes simultáneos |
| UI | UIButton, UIText, UITextField, UITextArea, UIImage, UISlider; foco por teclado/gamepad |
| Física | Colisiones 2D/3D (AABB, círculo); grilla de colisión y quadtree disponibles |
| Escenas | Sistema de gestor de escenas con transiciones, precarga de recursos y estado limpio |
| Configuración | Variables CMake por proyecto; sin recompilar el motor para cambiar parámetros del juego |

---

## 2. Arquitectura general

```
┌─────────────────────────────────────────────────────────────┐
│                    Proyecto del usuario                     │
│   (scenes/, entities/, assets/, project.cmake)              │
├─────────────────────────────────────────────────────────────┤
│                        CosmicEngine                         │
│                                                             │
│  ┌──────────┐  ┌──────────┐   ┌──────────┐  ┌───────────┐   │
│  │ Managers │  │  Models  │   │Collisions│  │Controllers│   │
│  └────┬─────┘  └────┬─────┘   └────┬─────┘  └─────┬─────┘   │
│       └─────────────┴──────────────┴──────────────┘         │
│                          │                                  │
│              ┌───────────▼───────────┐                      │
│              │      GameManager      │                      │
│              │  (bucle principal)    │                      │
│              └───────────────────────┘                      │
├─────────────────────────────────────────────────────────────┤
│              Librerías vendorizadas                         │
│  OpenGL (GLAD) · GLFW · FreeType · GLM · Assimp             │
│  ENet · miniaudio · nlohmann/json · Dear ImGui              │
└─────────────────────────────────────────────────────────────┘
```

### 2.1 Modos de compilación

El motor soporta exactamente dos configuraciones, seleccionadas en tiempo de compilación mediante la variable CMake `COSMIC_PROJECT_MODE`:

| Constante | Valor | Diferencias |
|---|---|---|
| `GAME_2D_CONFIGURATION` | `0` | Posiciones `vec2`, cámara ortográfica, colisiones 2D, iluminación 2D (32 luces) |
| `GAME_3D_CONFIGURATION` | `1` | Posiciones `vec3`, perspectiva, colisiones 3D, iluminación Phong 3D (8 luces), carga de modelos Assimp |

La macro de preprocesador `GAME_MODE_CONFIGURATION` selecciona los bloques condicionales `#if` en todos los headers del motor. Cambiar de modo requiere reconfigurar y recompilar.

### 2.2 Estructura de directorios del repositorio

```
Cosmic-Engine-Template/
├── CMakeLists.txt                  ← sistema de construcción raíz
├── CMakePresets.json               ← presets de configure y build
├── cmake/                          ← plantillas de archivos generados
│   ├── cosmic_project_entry.hpp.in
│   └── cosmic_project_config.hpp.in
├── default/                        ← recursos por defecto del motor
├── dll/                            ← DLLs de runtime Windows (libenet.dll)
├── include/
│   ├── CosmicEngine/               ← código fuente del motor
│   │   ├── interfaces/             ← definiciones, eventos, macros
│   │   ├── managers/               ← todos los gestores
│   │   ├── models/                 ← modelos de datos (Scene, Object, UI…)
│   │   ├── collisions/             ← sistemas de colisión
│   │   ├── controllers/camera/     ← controladores de cámara
│   │   └── utils/                  ← logging
│   ├── glm/, glad/, GLFW/, …       ← headers vendorizados
├── lib/                            ← librerías estáticas precompiladas
├── projects/                       ← proyectos de usuario
│   └── PuzzleRPG/                  ← demo 2D incluida
└── bin/                            ← salida de compilación por proyecto
```

---

## 3. Sistema de construcción (CMake)

### 3.1 Variables de `project.cmake`

Cada proyecto define un archivo `project.cmake` en su carpeta raíz. El CMakeLists.txt del motor lo incluye automáticamente al configurar con `-DCOSMIC_PROJECT=<nombre>`.

#### Variables obligatorias

| Variable | Descripción |
|---|---|
| `COSMIC_PROJECT_MODE` | `GAME_2D_CONFIGURATION` o `GAME_3D_CONFIGURATION` |
| `COSMIC_PROJECT_FIRST_SCENE_HEADER` | Ruta al header de la primera escena (relativa a `projects/<nombre>/`) |
| `COSMIC_PROJECT_FIRST_SCENE_CLASS` | Nombre cualificado de la clase C++ de la primera escena |

#### Variables opcionales

| Variable | Valor por defecto | Descripción |
|---|---|---|
| `COSMIC_PROJECT_NAME` | Nombre de la carpeta | Nombre lógico del proyecto |
| `COSMIC_PROJECT_EXECUTABLE_NAME` | `"COSMIC ENGINE"` | Nombre del ejecutable |
| `COSMIC_PROJECT_WINDOW_TITLE` | Valor de `NAME` | Título de la ventana |
| `COSMIC_PROJECT_WINDOW_ICON_PATH` | `""` | Icono de ventana (PNG) |
| `COSMIC_PROJECT_PROGRAM_ICON_PATH` | `""` | Icono del ejecutable (ICO, solo Windows) |
| `COSMIC_PROJECT_INITIAL_SCREEN_WIDTH` | `1920` | Ancho inicial de ventana en píxeles |
| `COSMIC_PROJECT_INITIAL_SCREEN_HEIGHT` | `1080` | Alto inicial de ventana |
| `COSMIC_PROJECT_BASE_RENDER_SCREEN_WIDTH` | `1920` | Ancho virtual base para cámara y UI |
| `COSMIC_PROJECT_BASE_RENDER_SCREEN_HEIGHT` | `1080` | Alto virtual base |
| `COSMIC_PROJECT_VIEWPORT_SCALE_TO_WINDOW` | `OFF` | El viewport escala al tamaño de la ventana |
| `COSMIC_PROJECT_WINDOW_RESIZABLE` | `ON` | La ventana es redimensionable |
| `COSMIC_PROJECT_WINDOW_LOCK_BASE_RENDER_ASPECT` | `OFF` | Bloquea la relación de aspecto al redimensionar |
| `COSMIC_PROJECT_START_FULLSCREEN` | `OFF` | Inicia en pantalla completa |
| `COSMIC_PROJECT_START_VSYNC` | `OFF` | VSync activo al iniciar |
| `COSMIC_PROJECT_ASSET_DIR` | `"assets"` | Directorio de assets relativo al proyecto |
| `COSMIC_PROJECT_SOURCE_FILES` | _(vacío)_ | Lista de archivos `.cpp` del proyecto |

### 3.2 Archivos generados por CMake

El sistema de construcción genera dos headers automáticamente en el directorio de build:

**`cosmic_project_entry.hpp`** — instancia la primera escena:
```cpp
inline CosmicEngine::Scene *CreateCosmicProjectFirstScene() {
    return new MyNamespace::MyScene();
}
```

**`cosmic_project_config.hpp`** — constantes del proyecto en tiempo de compilación:
```cpp
namespace CosmicEngine::ProjectConfig {
    inline constexpr const char* Name = "MiJuego";
    inline constexpr int InitialScreenWidth = 1920;
    inline constexpr bool StartFullscreen = false;
    // …
}
```

### 3.3 Presets disponibles

| Preset de configure | Preset de build | Plataforma |
|---|---|---|
| `windows-debug-puzzlerpg` | `build-windows-debug-puzzlerpg` | Windows / Debug |
| `windows-release-puzzlerpg` | `build-windows-release-puzzlerpg` | Windows / Release |
| `linux-debug-puzzlerpg` | `build-linux-debug-puzzlerpg` | Linux / Debug |
| `linux-release-puzzlerpg` | `build-linux-release-puzzlerpg` | Linux / Release |

Cada preset de configure fija `COSMIC_PROJECT`, `CMAKE_BUILD_TYPE`, `binaryDir` y el generador apropiado.

### 3.4 Directorios de salida

```
bin/<ProjectName>/<Platform>/<Configuration>/
    <ProjectName>.exe       ← ejecutable
    assets/                 ← assets copiados del proyecto
    *.dll                   ← DLLs de runtime (solo Windows)
```

---

## 4. Gestores (Managers)

Todos los gestores son singletons accesibles mediante macros alias definidas en `include/CosmicEngine/interfaces/definitions.hpp`. No es necesario incluir los headers de los gestores directamente si se usa la macro correspondiente.

| Gestor | Macro | Header |
|---|---|---|
| GameManager | `GM_MN` | `managers/game_manager.hpp` |
| SceneManager | `SCN_MN` | `managers/scene/scene_manager.hpp` |
| ObjectManager | `OBJ_MN` | `managers/object/object_manager.hpp` |
| ResourceManager | `RS_MN` | `managers/resource/resource_manager.hpp` |
| InputManager | `INP_MN` | `managers/input/input_manager.hpp` |
| CameraManager | `CAM_MN` | `managers/camera/camera_manager.hpp` |
| AudioManager | `AUD_MN` | `managers/audio/audio_manager.hpp` |
| LightManager | `LGT_MN` | `managers/light/light_manager.hpp` |
| UIManager | `UI_MN` | `managers/ui/ui_manager.hpp` |
| BodyManager | `BOD_MN` | `managers/body/body_manager.hpp` |
| AnimationManager | `ANM_MN` | `managers/animation/animation_manager.hpp` |
| NetworkManager | `NET_MN` | `managers/network/network_manager.hpp` |
| EventManager | `EVT_MN` | `managers/event/event_manager.hpp` |
| TimerManager | `TMR_MN` | `managers/timer/timer_manager.hpp` |
| ScheduleManager | `SCH_MN` | `managers/schedule/schedule_manager.hpp` |
| JsonManager | `JSON_MN` | `managers/storage/json/json_manager.hpp` |

### 4.1 GameManager (`GM_MN`)

Controla el bucle principal del motor. Administra la ventana GLFW, el contexto OpenGL y el tiempo de cuadro (`deltaTime`).

```cpp
GM_MN.SetWindowTitle("Mi Juego");
GM_MN.SetFullscreen(true);
GM_MN.SetVSync(true);
GM_MN.SetTargetFPS(60);
double dt = GM_MN.GetDeltaTime();
```

### 4.2 SceneManager (`SCN_MN`)

Gestiona la escena activa. Las transiciones se ejecutan al final del cuadro actual para evitar problemas de estado.

```cpp
SCN_MN.ReplaceScene(new GameScene());    // reemplaza inmediatamente (libera la actual)
SCN_MN.PushScene(new PauseScene());      // apila (la actual se pausa)
SCN_MN.PopScene();                       // vuelve a la escena anterior
SCN_MN.LoadSceneAsync(new BigScene());   // carga en hilo de fondo
```

### 4.3 ObjectManager (`OBJ_MN`)

Almacena y actualiza todos los objetos vivos. Mantiene la lista ordenada por `layerId` (orden de dibujado).

```cpp
OBJ_MN.Add(myObject);                   // registra y asigna ID único
OBJ_MN.Remove(myObject);               // marca para destrucción al final del cuadro
OBJ_MN.SortByLayer();                   // reordena la lista (llamar tras SetLayerId)
OBJ_MN.GetObjects();                    // vector<Object*> de todos los vivos
OBJ_MN.FindById(id);                    // búsqueda por ID
```

### 4.4 ResourceManager (`RS_MN`)

Carga y cachea todos los recursos de la GPU (texturas, fuentes, shaders). Gestiona el renderizado de sprites y texto.

```cpp
// Texturas
RS_MN.LoadTexture("key", "assets/textures/hero.png");
RS_MN.LoadSpriteSheet("sheet_key", "assets/sheet.png", rows, cols, padding);

// Fuentes (UTF-8 completo)
RS_MN.LoadFont("ui_font", "assets/fonts/myfont.ttf", 32);

// Animaciones
RS_MN.RegisterAnimationClip("run", "sheet_key", startFrame, frameCount, fps, loop);

// Renderizado 2D (llamar en draw())
RS_MN.Render2DSprite("key", position, size, rotation, color, alpha);
RS_MN.Render2DSpriteSheet("sheet_key", position, size, row, col, color, alpha);
RS_MN.RenderText("Hello", "ui_font", position, scale, color);
RS_MN.MeasureText("Hello", "ui_font", scale);   // retorna vec2 (ancho, alto)
```

### 4.5 InputManager (`INP_MN`)

Sistema de acciones con nombre. Los bindings son configurables en tiempo de ejecución y soportan teclado, ratón y gamepad simultáneamente.

```cpp
// Registrar acción
INP_MN.RegisterAction("jump", GLFW_KEY_SPACE);
INP_MN.AddGamepadBinding("jump", GLFW_GAMEPAD_BUTTON_A);

// Consultar estado
INP_MN.IsActionPressed("jump");    // activado este cuadro
INP_MN.IsActionHeld("jump");       // mantenido
INP_MN.IsActionReleased("jump");   // soltado este cuadro

// Ejes analógicos
INP_MN.GetAxis("move_x");          // float [-1, 1]
vec2 mouse = INP_MN.GetMousePosition();
```

### 4.6 CameraManager (`CAM_MN`)

Modo 2D: cámara ortográfica con zoom y seguimiento de objeto.  
Modo 3D: cámara de perspectiva libre con yaw/pitch.

```cpp
// 2D
CAM_MN.SetFocusPosition(vec2(400, 300));
CAM_MN.SetFocusObject(player, 0.0f, 0.0f);  // sigue al objeto
CAM_MN.SetZoom(1.5f);

// 3D
CAM_MN.SetFocusPosition(vec3(0, 5, 10), vec3(0, 0, 0));  // from, lookAt
CAM_MN.RotateBy(yaw, pitch);
```

### 4.7 AudioManager (`AUD_MN`)

Backend: miniaudio. Cada sonido se identifica por una clave de cadena.

```cpp
AUD_MN.Load("bgm", "assets/music/theme.ogg", SoundType::Music);
AUD_MN.Load("jump", "assets/sfx/jump.wav",   SoundType::SFX);

AUD_MN.Play("bgm", 0.8f, true);      // volumen 80%, en loop
AUD_MN.Play("jump", 1.0f, false);    // efecto de sonido

AUD_MN.SetMasterVolume(0.9f);
AUD_MN.SetMusicMasterVolume(0.7f);
AUD_MN.SetSfxMasterVolume(1.0f);

// Audio posicional
AUD_MN.PlayAt("footstep", position, SoundSpace::World2D, 1.0f, false);
AUD_MN.SetListenerPosition(listenerPos);
```

### 4.8 LightManager (`LGT_MN`)

Administra las luces de punto activas. El motor las transfiere al shader en cada cuadro.

```cpp
Light *torch = new Light(vec2(200, 300),
    vec2(0.05f), vec2(0.8f), vec2(0.5f),   // ambient, diffuse, specular
    1.0f, 0.09f, 0.032f,                    // constante, lineal, cuadrática
    64.0f, true, LightMobility::Dynamic);
torch->SetColorTint(vec3(1.0f, 0.6f, 0.2f));  // tinte naranja
LGT_MN.Add(torch);
```

Límites por modo: 32 luces en 2D, 8 en 3D.

### 4.9 UIManager (`UI_MN`)

Gestiona elementos de UI. Maneja el foco de teclado/gamepad automáticamente y distingue el modo ratón del modo teclado.

```cpp
UIButton *btn = new UIButton("Jugar", "ui_font", "button_idle",
                              vec2(760, 500), vec2(400, 60));
btn->SetHoverTexture("button_hover");
btn->SetOnClick([]() { SCN_MN.ReplaceScene(new GameScene()); });
UI_MN.AddElement(btn);

// Entrada de texto
UITextField *field = new UITextField("", "Nombre…", "ui_font",
                                      vec2(760, 600), vec2(400, 50), 32);
field->SetOnSubmit([field]() { /* usar field->GetText() */ });
UI_MN.AddElement(field);
```

### 4.10 NetworkManager (`NET_MN`)

Backend: ENet (UDP confiable). Diseñado para arquitectura host-autoritativa.

```cpp
// Host
NET_MN.StartServer(25566);

// Cliente
NET_MN.ConnectToServer("192.168.1.10", 25566, "JugadorUno");

// Registrar handler
NET_MN.RegisterHandler(Net::Msg_PlayerState,
    [](int peerId, const nlohmann::json &payload) {
        int x = payload.value("x", 0);
        // aplicar estado…
    });

// Enviar mensaje
nlohmann::json msg;
msg["x"] = player.x;
msg["y"] = player.y;
NET_MN.SendToServer({Net::Msg_PlayerState, msg});
NET_MN.Broadcast({Net::Msg_WorldState, worldJson});
```

Los tipos de mensaje del usuario deben usar valores `>= 100` para evitar colisiones con los mensajes internos del motor.

---

## 5. Modelos de datos

### 5.1 Scene

Clase abstracta base de toda escena. El desarrollador hereda de ella y sobrescribe los métodos virtuales puros.

```cpp
class MyScene : public CosmicEngine::Scene {
public:
    MyScene() : Scene("MyScene") {}
    void loadResources() override;  // cargar texturas, fuentes, audio
    void init() override;           // crear objetos, registrar handlers, construir UI
    void update(double dt) override; // lógica por cuadro
    void draw() override;           // renderizado adicional (opcional)
    void reset() override;          // limpiar estado al reutilizar la escena (opcional)
};
```

El orden de llamadas garantizado por el motor es: `loadResources()` → `init()` → bucle `[update() → draw()]` → destructor.

Herramientas de debug heredadas: `ToogleShowBodys()`, `ToogleShowGrid()`, `ToogleShowCamera()`.

### 5.2 Object

Clase base de todas las entidades del juego. Mantenida por `OBJ_MN`.

**Constructor 2D:**
```cpp
Object(string className, vec2 position, vec2 size,
       float rotation = 0.0f, short layerId = 0);
```

**Constructor 3D:**
```cpp
Object(string className, vec3 position, vec3 size,
       vec3 rotation = vec3(0.0f));
```

**API común:**
```cpp
SetPosition / GetPosition
SetSize / GetSize
SetRotation / GetRotation
SetVelocity / GetVelocity
SetMaxVelocity / SetMinVelocity
SetDirection / GetDirection    // vec normalizado de movimiento
SetColor / GetColor            // tinte RGB
SetVisible / GetVisible
Destroy()                      // marca para eliminación al final del cuadro
IsAlive()
Clone()                        // copia superficial
GetClassName() / GetID()
```

**Solo 2D:** `SetLayerId(short)` / `GetLayerId()` — controla el orden de dibujado. Valores típicos: 0 = suelo, 1 = muros, 2 = entidades, 100 = siempre al frente.

**Macro de downcast seguro:**
```cpp
IF_GET_TYPE(obj, EnemyObject, enemy) {
    enemy->Attack();  // solo ejecuta si el cast tiene éxito
}
```

### 5.3 Body

Componente de colisión adjunto a un `Object`. El callback se dispara cuando se detecta una colisión.

```cpp
// En el constructor de la entidad:
body = new Body(this, vec2(0, 0), size,
    CALLBACK_COLLISION_EVENT(MyEntity::OnCollision));
BOD_MN.Add(body);

void MyEntity::OnCollision(Object *other, BodyCollisionSide side) {
    if (side == BodyCollisionSide::LEFT) { /* resolución */ }
}
```

Tipos 2D: `BodyType::Rectangle`, `BodyType::Circle`, `BodyType::Triangle`.

### 5.4 Componentes de UI

Todos los elementos UI heredan de `UIElement` y se registran en `UI_MN`. Son gestionados y renderizados automáticamente.

| Clase | Uso principal |
|---|---|
| `UIButton` | Botón interactivo con texto, textura de hover y callback onClick |
| `UIText` | Etiqueta de texto; soporta multilínea, escala y alineación izquierda |
| `UITextField` | Campo de entrada de una línea; UTF-8 completo, placeholder, callbacks |
| `UITextArea` | Área de texto multilínea con scroll vertical y word-wrap |
| `UIImage` | Imagen UI con tinte y alpha |
| `UISlider` | Control deslizante con rango, paso, formateador y callbacks |

Todos soportan: `SetPosition`, `SetSize`, `SetVisible`, jerarquía padre/hijo (`AddChild`, `SetParent`), y foco por teclado (`SetFocusable`, `SetFocused`).

---

## 6. Sistemas de colisión

### 6.1 Grilla de colisión (`GameGridCollisions`)

Divide el espacio en celdas de tamaño fijo. Eficiente para mundos con objetos distribuidos uniformemente.

```cpp
#include GAMEGRIDCOLLISIONS_HEADER
GameGridCollisions grid(cellWidth, cellHeight);
grid.Update();       // llamar en update() antes de leer colisiones
```

### 6.2 Quadtree (`GameQuadTreeCollisions`)

Subdivisión recursiva del espacio. Eficiente para objetos distribuidos de forma irregular.

### 6.3 Sistema de Bodies

La detección automática se activa cuando los objetos tienen un `Body` registrado en `BOD_MN`. El `BodyManager` resuelve las colisiones y dispara los callbacks en cada cuadro.

---

## 7. Sistema de iluminación

### 7.1 Modo 2D

El shader lit 2D calcula la contribución de cada luz usando atenuación cuadrática. El efecto final es:

1. Se aplica un color ambiental nocturno base (`nightColor`).
2. Cada luz de punto restaura el albedo original del sprite en su radio de influencia.
3. Se acumula el color ponderado de las luces para obtener un tinte resultante.
4. Se permite leve overbright para efectos de luces intensas.

```cpp
Light *fire = new Light(position, vec2(0.02f), vec2(1.0f), vec2(0.5f));
fire->SetColorTint(vec3(1.0f, 0.4f, 0.1f));   // naranja fuego
fire->SetLinearLight(0.05f);
fire->SetQuadraticLight(0.01f);
LGT_MN.Add(fire);
```

### 7.2 Modo 3D

Iluminación Phong clásica: componente ambiental + difusa + especular por cada luz de punto. El parámetro `shininess` controla el tamaño del brillo especular.

---

## 8. Sistema de red

### 8.1 Arquitectura

El motor implementa un modelo **host-autoritativo**: el host ejecuta toda la lógica del juego y es la única fuente de verdad. Los clientes envían *intenciones* y aplican el estado que el host confirma.

### 8.2 Mensajes

Todos los mensajes son structs con un campo `type` (entero) y un `payload` (objeto JSON de nlohmann/json). El motor reserva los tipos 0–99; el proyecto del usuario puede usar 100 en adelante.

```cpp
struct NetworkMessage {
    int type;
    nlohmann::json payload;
};
```

### 8.3 Flujo de conexión

```
Host                                Cliente
  │ NET_MN.StartServer(port)           │
  │                                    │ NET_MN.ConnectToServer(ip, port, user)
  │ ← ClientConnect                   │
  │ AssignClientId →                  │
  │ ← [mensajes de juego]             │
  │ → [estado replicado]              │
```

---

## 9. Pipeline de renderizado

El motor usa un pipeline de doble buffer estándar (GLFW). El orden de renderizado por cuadro es:

1. `glClear` con el color de fondo.
2. Iteración de objetos ordenados por `layerId` (ascendente) → llamada a `draw()` de cada objeto.
3. Renderizado de UI (coordenadas de pantalla, sin transformación de cámara).
4. `glfwSwapBuffers`.

Los shaders 2D están embebidos en `master_shaders.hpp` (lit y unlit). Los shaders 3D están en el mismo archivo, separados por bloques `#if GAME_3D_CONFIGURATION`.

---

## 10. Librerías vendorizadas

| Librería | Versión incluida | Uso |
|---|---|---|
| GLFW | 3.x | Ventana, contexto OpenGL, entrada |
| GLAD | — | Loader de funciones OpenGL 3.3 |
| GLM | 0.9.x | Álgebra lineal (vec2/3/4, mat4, quaternion) |
| FreeType | 2.x | Rasterizado de fuentes TrueType |
| Assimp | 5.x | Importación de modelos 3D |
| ENet | 1.3.x | Networking UDP confiable |
| miniaudio | 0.11.x | Audio multiplataforma |
| nlohmann/json | 3.x | Serialización/deserialización JSON |
| Dear ImGui | 1.9x | UI de depuración (disponible, no activa por defecto) |

Todas las librerías están precompiladas en `lib/` para Windows (MinGW UCRT64). En Linux se instalan mediante el gestor de paquetes del sistema.

---

## 11. Limitaciones conocidas

| Área | Limitación |
|---|---|
| Luces 2D | Máximo 32 por escena (constante en shader) |
| Luces 3D | Máximo 8 por escena |
| Red | Sin cifrado; comunicación en texto plano (JSON) |
| Audio | Un canal de música por clave; sin mezcla multicapa |
| Plataforma | Windows y Linux únicamente (sin soporte macOS) |
| Física | Sin simulación de cuerpo rígido; solo detección y callback de colisión |
