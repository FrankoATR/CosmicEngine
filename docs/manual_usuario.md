# Manual de Usuario — CosmicEngine

> **Versión del documento:** 1.0  
> **Audiencia:** Desarrolladores C++ que desean crear un videojuego con CosmicEngine  
> **Requisito de conocimiento previo:** C++17/20, programación orientada a objetos

---

## 1. Introducción

Este manual describe cómo instalar, configurar y usar CosmicEngine para crear un videojuego desde cero. CosmicEngine es un framework de videojuegos en C++ con OpenGL. Abstraye la complejidad de las APIs gráficas, de audio, de red y de entrada, y expone un modelo de programación basado en escenas, objetos y gestores.

El desarrollador crea un **proyecto** dentro de la carpeta `projects/`, define su primera escena, conecta sus entidades con los gestores del motor y obtiene como resultado un ejecutable con todos los assets copiados.

---

## 2. Instalación del entorno de desarrollo

### 2.1 Windows (recomendado: MSYS2 UCRT64)

1. Descarga e instala [MSYS2](https://www.msys2.org/).
2. Abre la terminal **MSYS2 UCRT64** (no MINGW64 ni MSYS).
3. Instala las dependencias:

```bash
pacman -Syu
pacman -S --needed \
  mingw-w64-ucrt-x86_64-gcc \
  mingw-w64-ucrt-x86_64-cmake \
  mingw-w64-ucrt-x86_64-make
```

4. Verifica que las herramientas estén en el `PATH`:

```bash
gcc --version
cmake --version
mingw32-make --version
```

> **Nota:** Usa siempre la terminal UCRT64. Si compilas desde VS Code, configura el entorno de la terminal integrada para que cargue UCRT64.

### 2.2 Linux (Debian/Ubuntu)

```bash
sudo apt update
sudo apt install build-essential cmake pkg-config \
    libglfw3-dev libfreetype6-dev zlib1g-dev \
    libassimp-dev libgl1-mesa-dev
```

### 2.3 Clonar el repositorio

```bash
git clone <url-del-repositorio> CosmicEngine
cd CosmicEngine
```

Las librerías vendorizadas ya están precompiladas en `lib/`, no requieren instalación adicional.

---

## 3. Crear un proyecto nuevo

### 3.1 Estructura mínima recomendada

```
projects/
  MiJuego/
    project.cmake          ← configuración del proyecto
    scenes/
      main_scene.hpp
      main_scene.cpp
    entities/              ← clases que heredan de Object
    assets/
      textures/
      fonts/
      audio/
```

### 3.2 El archivo `project.cmake`

Este archivo define todos los parámetros del proyecto. Las variables marcadas como obligatorias no tienen valor por defecto y el build fallará si se omiten.

**Ejemplo completo:**

```cmake
# --- Obligatorias ---
set(COSMIC_PROJECT_MODE "GAME_2D_CONFIGURATION")
set(COSMIC_PROJECT_FIRST_SCENE_HEADER "scenes/main_scene.hpp")
set(COSMIC_PROJECT_FIRST_SCENE_CLASS  "MiJuego::MainScene")

# --- Opcionales (con sus valores predeterminados) ---
set(COSMIC_PROJECT_NAME                        "MiJuego")
set(COSMIC_PROJECT_EXECUTABLE_NAME             "MiJuego")
set(COSMIC_PROJECT_WINDOW_TITLE                "Mi Juego")
set(COSMIC_PROJECT_INITIAL_SCREEN_WIDTH        1280)
set(COSMIC_PROJECT_INITIAL_SCREEN_HEIGHT        720)
set(COSMIC_PROJECT_BASE_RENDER_SCREEN_WIDTH    1280)
set(COSMIC_PROJECT_BASE_RENDER_SCREEN_HEIGHT    720)
set(COSMIC_PROJECT_WINDOW_RESIZABLE             ON)
set(COSMIC_PROJECT_START_VSYNC                  ON)
set(COSMIC_PROJECT_ASSET_DIR                  "assets")

# --- Archivos fuente del proyecto ---
set(COSMIC_PROJECT_SOURCE_FILES
    scenes/main_scene.cpp
)
```

> **Consejo:** `COSMIC_PROJECT_BASE_RENDER_SCREEN_WIDTH/HEIGHT` define el "canvas virtual" para el sistema de cámara y UI. Las posiciones de la UI y la cámara se expresan en esas coordenadas, no en las de la ventana física.

### 3.3 Registrar el proyecto en CMakePresets.json

Agrega dos entradas al archivo `CMakePresets.json` en la raíz del repositorio: un preset de configure y uno de build.

```json
{
  "name": "windows-debug-mijuego",
  "displayName": "Windows Debug — MiJuego",
  "inherits": "base-windows-debug",
  "cacheVariables": {
    "COSMIC_PROJECT": "MiJuego"
  },
  "binaryDir": "${sourceDir}/build/windows-debug-mijuego"
}
```

```json
{
  "name": "build-windows-debug-mijuego",
  "displayName": "Build Windows Debug — MiJuego",
  "configurePreset": "windows-debug-mijuego"
}
```

---

## 4. Crear la primera escena

Toda escena hereda de `CosmicEngine::Scene`. El motor garantiza el siguiente ciclo de vida:

```
loadResources() → init() → [update(dt) → draw()] en bucle → destructor
```

**`scenes/main_scene.hpp`:**

```cpp
#pragma once
#include <CosmicEngine/models/scene/scene.hpp>

namespace MiJuego {

class MainScene : public CosmicEngine::Scene {
public:
    MainScene();
    void loadResources() override;
    void init() override;
    void update(double dt) override;
    void draw() override;
};

} // namespace MiJuego
```

**`scenes/main_scene.cpp`:**

```cpp
#include "main_scene.hpp"
#include <CosmicEngine/interfaces/definitions.hpp>  // macros de gestores

namespace MiJuego {

MainScene::MainScene() : Scene("MainScene") {}

void MainScene::loadResources()
{
    RS_MN.LoadTexture("hero", "assets/textures/hero.png");
    RS_MN.LoadFont("ui_font", "assets/fonts/roboto.ttf", 28);
    AUD_MN.Load("bgm", "assets/audio/theme.ogg", SoundType::Music);
}

void MainScene::init()
{
    AUD_MN.Play("bgm", 0.7f, true);
}

void MainScene::update(double dt)
{
    if (INP_MN.IsActionPressed("quit"))
        GM_MN.Quit();
}

void MainScene::draw()
{
    RS_MN.RenderText("Hola, CosmicEngine!", "ui_font", {100, 100}, 1.0f, {1,1,1});
}

} // namespace MiJuego
```

---

## 5. Crear entidades

Las entidades del juego heredan de `CosmicEngine::Object`. Hay dos constructores disponibles según el modo de compilación.

### 5.1 Entidad 2D básica

```cpp
#pragma once
#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/interfaces/definitions.hpp>

class Hero : public CosmicEngine::Object {
public:
    Hero(glm::vec2 pos)
        : Object("Hero", pos, {32, 32}, 0.0f, /*layerId=*/2) {}

    void update(double dt) override
    {
        glm::vec2 dir{0, 0};
        if (INP_MN.IsActionHeld("move_right")) dir.x =  1;
        if (INP_MN.IsActionHeld("move_left"))  dir.x = -1;
        if (INP_MN.IsActionHeld("move_up"))    dir.y = -1;
        if (INP_MN.IsActionHeld("move_down"))  dir.y =  1;
        SetPosition(GetPosition() + dir * 200.0f * (float)dt);
    }

    void draw() override
    {
        RS_MN.Render2DSprite("hero", GetPosition(), GetSize(), GetRotation());
    }
};
```

### 5.2 Registrar la entidad en la escena

```cpp
void MainScene::init()
{
    // Registrar acciones de entrada
    INP_MN.RegisterAction("move_right", GLFW_KEY_RIGHT);
    INP_MN.RegisterAction("move_left",  GLFW_KEY_LEFT);
    INP_MN.RegisterAction("move_up",    GLFW_KEY_UP);
    INP_MN.RegisterAction("move_down",  GLFW_KEY_DOWN);
    INP_MN.RegisterAction("quit",       GLFW_KEY_ESCAPE);

    // Crear y registrar la entidad
    auto *hero = new Hero({400, 300});
    OBJ_MN.Add(hero);
}
```

> **Importante:** Todo objeto debe ser registrado con `OBJ_MN.Add()` para que el motor lo actualice y destruya correctamente. No lo elimines con `delete` directamente — usa `object->Destroy()`.

### 5.3 Orden de dibujado (`layerId`)

El motor dibuja los objetos de menor a mayor `layerId`. Convención recomendada:

| layerId | Uso |
|---|---|
| 0 | Suelo, fondos |
| 1 | Muros, obstáculos estáticos |
| 2 | Entidades móviles, ítems |
| 10+ | Efectos, partículas |
| 100+ | HUD y efectos siempre al frente |

Si cambias el `layerId` de un objeto en tiempo de ejecución, llama a `OBJ_MN.SortByLayer()` una vez para reordenar la lista.

---

## 6. Gestión de recursos

Todos los recursos se cargan una vez en `loadResources()` y se acceden durante el juego mediante su **clave** (una cadena que tú eliges).

### 6.1 Texturas

```cpp
// Textura simple
RS_MN.LoadTexture("clave", "assets/textures/imagen.png");

// Spritesheet (filas, columnas, padding)
RS_MN.LoadSpriteSheet("personaje", "assets/textures/sprite.png", 4, 8, 0);
```

**Renderizar:**
```cpp
// Textura simple: posición, tamaño, rotación, color, alpha
RS_MN.Render2DSprite("clave", pos, size, 0.0f, {1,1,1}, 1.0f);

// Spritesheet: especifica fila y columna del frame
RS_MN.Render2DSpriteSheet("personaje", pos, size, /*row=*/0, /*col=*/3);
```

### 6.2 Fuentes y texto

```cpp
RS_MN.LoadFont("font_key", "assets/fonts/mifuente.ttf", /*puntaje=*/32);

RS_MN.RenderText("Hola!", "font_key", pos, /*escala=*/1.0f, {1, 1, 1});

// Medir el texto antes de dibujarlo
glm::vec2 textSize = RS_MN.MeasureText("Hola!", "font_key", 1.0f);
```

> Las fuentes soportan todos los caracteres del Suplemento Latin-1 (U+00A0–U+00FF), incluyendo vocales con tilde, ñ, ü y similares. El texto debe estar en UTF-8.

---

## 7. Sistema de entrada

El motor usa un sistema de **acciones con nombre**. Los bindings se registran una vez y el resto del código solo consulta el nombre de la acción.

```cpp
// Registrar (hacer esto en init() o al inicio del juego)
INP_MN.RegisterAction("jump",      GLFW_KEY_SPACE);
INP_MN.RegisterAction("attack",    GLFW_KEY_Z);
INP_MN.AddGamepadBinding("jump",   GLFW_GAMEPAD_BUTTON_A);
INP_MN.AddGamepadBinding("attack", GLFW_GAMEPAD_BUTTON_X);

// Consultar en update()
if (INP_MN.IsActionPressed("jump"))    { /* activado este cuadro */ }
if (INP_MN.IsActionHeld("attack"))     { /* mantenido */ }
if (INP_MN.IsActionReleased("jump"))   { /* soltado este cuadro */ }

// Ejes analógicos (teclado también funciona como [-1, 0, 1])
float h = INP_MN.GetAxis("move_x");

// Ratón
glm::vec2 mousePos = INP_MN.GetMousePosition();
bool leftClick = INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
```

---

## 8. Cámara

### 8.1 Modo 2D

```cpp
// Posición fija
CAM_MN.SetFocusPosition(glm::vec2(400, 300));

// Seguir un objeto (offsetX, offsetY desde el centro del objeto)
CAM_MN.SetFocusObject(heroPtr, 0.0f, 0.0f);

// Zoom (1.0 = normal, >1 = acercado, <1 = alejado)
CAM_MN.SetZoom(1.5f);
```

### 8.2 Modo 3D

```cpp
// Posición y punto de mira
CAM_MN.SetFocusPosition(glm::vec3(0, 5, 10), glm::vec3(0, 0, 0));

// Rotación libre
CAM_MN.RotateBy(yaw, pitch);
```

---

## 9. Audio

```cpp
// Cargar
AUD_MN.Load("bgm",    "assets/music/tema.ogg",     SoundType::Music);
AUD_MN.Load("jump",   "assets/sfx/salto.wav",      SoundType::SFX);
AUD_MN.Load("steps",  "assets/sfx/pasos.wav",      SoundType::SFX);

// Reproducir (volumen normalizado 0.0–1.0, loop)
AUD_MN.Play("bgm",  0.7f, true);    // música en loop
AUD_MN.Play("jump", 1.0f, false);   // efecto de sonido sin loop

// Control de volumen global
AUD_MN.SetMasterVolume(0.9f);
AUD_MN.SetMusicMasterVolume(0.6f);
AUD_MN.SetSfxMasterVolume(1.0f);

// Audio posicional (2D espacial)
AUD_MN.PlayAt("steps", entityPosition, SoundSpace::World2D, 1.0f, false);
AUD_MN.SetListenerPosition(cameraPosition);

// Parar / pausar
AUD_MN.Stop("bgm");
AUD_MN.Pause("bgm");
AUD_MN.Resume("bgm");
```

---

## 10. Interfaz de usuario (UI)

Los componentes de UI son objetos administrados por `UI_MN`. Se crean, se configuran y se registran en `init()`.

### 10.1 UIButton

```cpp
auto *btn = new UIButton("Jugar", "ui_font", "btn_idle",
                          glm::vec2(640, 400), glm::vec2(200, 50));
btn->SetHoverTexture("btn_hover");
btn->SetOnClick([]() {
    SCN_MN.ReplaceScene(new GameScene());
});
UI_MN.AddElement(btn);
```

### 10.2 UIText

```cpp
auto *label = new UIText("Puntuación: 0", "ui_font",
                           glm::vec2(20, 20), 1.0f, glm::vec3(1,1,1));
UI_MN.AddElement(label);

// Actualizar el texto en update():
label->SetText("Puntuación: " + std::to_string(score));
```

### 10.3 UITextField

```cpp
auto *field = new UITextField("", "Escribe tu nombre…", "ui_font",
                               glm::vec2(640, 500), glm::vec2(300, 40), /*maxLen=*/20);
field->SetOnSubmit([field]() {
    std::string nombre = field->GetText();
    // usar nombre…
});
UI_MN.AddElement(field);
```

### 10.4 UISlider

```cpp
auto *volSlider = new UISlider("ui_font", glm::vec2(640, 550), glm::vec2(300, 30),
                                /*min=*/0.0f, /*max=*/1.0f, /*step=*/0.05f);
volSlider->SetValue(0.8f);
volSlider->SetOnChange([](float val) {
    AUD_MN.SetMasterVolume(val);
});
UI_MN.AddElement(volSlider);
```

### 10.5 Foco por teclado/gamepad

La UI detecta automáticamente si el usuario está usando teclado/gamepad y muestra un indicador de foco. Para navegar entre elementos con Tab/Shift-Tab o con el D-Pad del gamepad, marca los elementos como enfocables:

```cpp
btn->SetFocusable(true);
field->SetFocusable(true);
```

El primer elemento enfocable registrado recibe el foco inicial.

---

## 11. Iluminación dinámica (modo 2D)

La iluminación 2D aplica luces de punto sobre los sprites. Sin luces, el nivel aparece completamente oscuro si el shader "lit" está activo.

```cpp
// Crear una antorcha
auto *fire = new Light(
    glm::vec2(300, 200),       // posición
    glm::vec2(0.03f),          // ambient
    glm::vec2(1.0f),           // diffuse
    glm::vec2(0.5f),           // specular
    1.0f, 0.09f, 0.032f,       // constante, lineal, cuadrática
    64.0f,                     // shininess
    true,                      // habilitada
    LightMobility::Dynamic     // puede moverse en runtime
);
fire->SetColorTint(glm::vec3(1.0f, 0.5f, 0.1f));  // tinte naranja
LGT_MN.Add(fire);
```

Para mover la luz cada cuadro (ej.: adjunta al jugador):
```cpp
fire->SetPosition(hero->GetPosition());
```

Límite: **32 luces simultáneas** en modo 2D.

---

## 12. Colisiones

### 12.1 Agregar un Body a una entidad

```cpp
#include <CosmicEngine/models/body/body.hpp>

// En el constructor de Hero:
body = new CosmicEngine::Body(
    this,
    glm::vec2(0, 0),           // offset desde el centro del objeto
    GetSize(),                 // tamaño del collider
    CALLBACK_COLLISION_EVENT(Hero::OnCollision)
);
BOD_MN.Add(body);
```

```cpp
void Hero::OnCollision(CosmicEngine::Object *other, BodyCollisionSide side)
{
    if (side == BodyCollisionSide::BOTTOM) {
        SetVelocity({GetVelocity().x, 0});  // detener caída
    }
}
```

> El `BodyManager` resuelve las colisiones y llama a los callbacks automáticamente cada cuadro.

---

## 13. Red multijugador

### 13.1 Iniciar un host (servidor)

```cpp
NET_MN.StartServer(25566);

// Escuchar conexiones de clientes
NET_MN.RegisterHandler(Net::Msg_ClientConnect,
    [](int peerId, const nlohmann::json &payload) {
        std::string usuario = payload.value("username", "anon");
        // registrar jugador…
    });
```

### 13.2 Conectar como cliente

```cpp
NET_MN.ConnectToServer("192.168.1.10", 25566, "MiNombre");
```

### 13.3 Definir y enviar mensajes propios

Usa tipos de mensaje con valor `>= 100` para no colisionar con los internos del motor.

```cpp
// En un header compartido entre escenas:
constexpr int Msg_MovePlayer = 100;
constexpr int Msg_WorldState = 101;

// Enviar desde el cliente al host:
nlohmann::json data;
data["x"] = hero->GetPosition().x;
data["y"] = hero->GetPosition().y;
NET_MN.SendToServer({Msg_MovePlayer, data});

// Enviar desde el host a todos los clientes:
NET_MN.Broadcast({Msg_WorldState, worldData});

// Registrar handler en el host:
NET_MN.RegisterHandler(Msg_MovePlayer,
    [this](int peerId, const nlohmann::json &payload) {
        float x = payload.value("x", 0.0f);
        float y = payload.value("y", 0.0f);
        players[peerId]->SetPosition({x, y});
    });
```

---

## 14. Persistencia de datos (JSON)

```cpp
// Guardar datos
nlohmann::json config;
config["volume"] = 0.8f;
config["fullscreen"] = false;
JSON_MN.Save("user_config", config, "saves/config.json");

// Cargar datos
nlohmann::json loaded;
if (JSON_MN.Load("user_config", loaded, "saves/config.json")) {
    float vol = loaded.value("volume", 1.0f);
    AUD_MN.SetMasterVolume(vol);
}
```

---

## 15. Cambiar de escena

```cpp
// Reemplazar la escena actual (libera la vieja)
SCN_MN.ReplaceScene(new MenuScene());

// Apilar una escena encima (la actual se pausa pero no se destruye)
SCN_MN.PushScene(new PauseScene());

// Volver a la escena anterior (cierra la actual)
SCN_MN.PopScene();
```

> Los recursos cargados en `loadResources()` son gestionados por `RS_MN` y sobreviven los cambios de escena, a menos que se descarguen explícitamente. Si dos escenas usan los mismos recursos con la misma clave, no se duplican.

---

## 16. Compilar y ejecutar

### 16.1 Windows

```bash
# Desde una terminal MSYS2 UCRT64
cmake --preset windows-debug-mijuego
cmake --build --preset build-windows-debug-mijuego -j8
.\bin\MiJuego\windows\Debug\MiJuego.exe
```

```bash
# Versión release optimizada
cmake --preset windows-release-mijuego
cmake --build --preset build-windows-release-mijuego -j8
.\bin\MiJuego\windows\Release\MiJuego.exe
```

### 16.2 Linux

```bash
cmake --preset linux-debug-mijuego
cmake --build --preset build-linux-debug-mijuego -j8
./bin/MiJuego/linux/Debug/MiJuego
```

> **Importante:** Ejecuta siempre el binario **desde su directorio de salida** (`bin/…`), no desde la raíz del repositorio. Los assets y las DLLs están en ese directorio.

---

## 17. Depuración

La clase `Scene` hereda métodos de ayuda para visualizar información de debug en tiempo de ejecución:

```cpp
ToogleShowBodys();    // muestra los colliders de todos los Bodies
ToogleShowGrid();     // muestra la grilla de colisión
ToogleShowCamera();   // muestra los límites del frustum de la cámara
```

Actívalos con una tecla en `update()`:

```cpp
if (INP_MN.IsActionPressed("debug"))
    ToogleShowBodys();
```

---

## 18. Guía de resolución de problemas

| Problema | Causa probable | Solución |
|---|---|---|
| El ejecutable no abre | Falta una DLL en el directorio de salida | Verifica que `dll/` contenga `libenet.dll` y que se copió al hacer build |
| Los assets no se cargan | Ruta relativa incorrecta | Asegúrate de ejecutar el .exe desde `bin/…/` no desde la raíz |
| El texto muestra `?` o caracteres rotos | La fuente no tiene el glyph o el texto no es UTF-8 | Usa una fuente con cobertura Latin-1 (ej.: monogram-extended) |
| `cmake --preset` falla | El preset no existe o el nombre del proyecto no coincide | Revisa `CMakePresets.json` y que `COSMIC_PROJECT` coincide con el nombre de la carpeta |
| El audio no suena | El archivo no se cargó o la ruta es incorrecta | Comprueba que `AUD_MN.Load()` recibe la ruta correcta relativa al directorio de ejecución |
| La cámara no sigue al jugador | `SetFocusObject` con puntero nulo | Llama a `SetFocusObject` después de crear el objeto |
| Colisiones no detectadas | El `Body` no fue registrado con `BOD_MN.Add()` | Agrega `BOD_MN.Add(body)` en el constructor de la entidad |

---

## 19. Flujo de trabajo resumido

```
1. Crear projects/MiJuego/project.cmake
2. Implementar la primera escena (MainScene)
3. Agregar el preset en CMakePresets.json
4. cmake --preset windows-debug-mijuego
5. cmake --build --preset build-windows-debug-mijuego
6. Ejecutar bin/MiJuego/windows/Debug/MiJuego.exe
7. Iterar: editar → compilar → probar
```

Los assets se copian automáticamente al directorio de salida en cada compilación, por lo que los cambios en `assets/` se reflejan en el siguiente build sin pasos adicionales.
