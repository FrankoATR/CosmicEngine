# README Técnico de CosmicEngine

## Propósito del documento

Este documento describe, en español y en tercera persona, el trabajo técnico realizado sobre CosmicEngine durante la sesión. El enfoque está puesto en cambios de infraestructura del motor: capa de red, sistema de entrada, subsistema UI, correcciones del pipeline de render para UI en 3D, ajustes del administrador de cámara y adaptación del sistema de build para Linux.

El documento excluye deliberadamente la descripción funcional de escenas de demostración, lógica de juego o mecánicas específicas. Lo que se documenta aquí es el cambio técnico aplicado al motor y a sus sistemas compartidos.

## 1. Refactorización de la capa de red

### 1.1 Objetivo técnico

La capa de red fue reorganizada para separar responsabilidades que antes estaban demasiado concentradas en un flujo único. El objetivo principal fue convertir el subsistema de red en una capa de transporte y despacho reutilizable, menos acoplada al código consumidor y más consistente en su manejo de mensajes, peers y ciclo de vida.

### 1.2 Componentes lógicos involucrados

La refactorización consolidó tres piezas principales:

- `NetworkMessage`: representa un mensaje de red tipado, con un `NetMessageType` y un payload estructurado.
- `NetworkSession`: encapsula estado de sesión, peer y contexto de conexión.
- `NetworkManager`: centraliza inicialización, conexión, envío, recepción, broadcast, despacho por tipo y limpieza.

Con esta separación, el `NetworkManager` deja de ser un bloque monolítico y pasa a funcionar como coordinador del transporte, mientras que el detalle semántico del mensaje se mueve a `NetworkMessage` y el estado de conexión se encapsula en `NetworkSession`.

### 1.3 Modelo de mensaje

El trabajo formalizó el uso de una estructura de mensaje apoyada en `NetMessageType`. Esta decisión tiene una consecuencia importante a nivel de diseño: el código deja de depender de cadenas o paquetes ambiguos y pasa a apoyarse en un contrato tipado para cada evento de red.

La lógica interna queda dividida en dos capas:

- Capa de transporte: recibe bytes, serializa o deserializa y entrega mensajes al motor.
- Capa de consumo: interpreta `type` y `payload` mediante handlers registrados.

Esto permite que el motor soporte crecimiento futuro sin convertir el `NetworkManager` en una cadena de `if` o `switch` acoplados a escenas específicas.

### 1.4 Registro y despacho de handlers

Uno de los cambios más relevantes fue la lógica de handlers por tipo de mensaje. En lugar de procesar todos los mensajes dentro de una ruta cerrada, el manager expone un modelo de suscripción que permite:

- registrar handlers por tipo
- limpiar handlers cuando cambian los consumidores
- desacoplar el transporte del comportamiento de alto nivel

La lógica buscada es la siguiente:

1. El mensaje entra por ENet.
2. Se deserializa hacia `NetworkMessage`.
3. El `NetworkManager` consulta el tipo.
4. Se invocan los callbacks asociados a dicho tipo.

Este patrón convierte la capa de red en una infraestructura orientada a eventos. Técnicamente, esto reduce acoplamiento, simplifica teardown y evita que el manager tenga conocimiento directo del comportamiento específico de quien consume la red.

### 1.5 Métodos y responsabilidades del manager

Durante la refactorización se trabajó sobre la lógica de métodos equivalentes a estas operaciones:

- inicialización del subsistema de red
- `StartServer()` para crear host ENet y preparar recepción de peers
- `ConnectToServer()` para resolver dirección, crear peer y comenzar la sesión cliente
- `ProcessMessages()` para extraer eventos desde ENet y convertirlos en mensajes despachables
- `Broadcast()` para enviar a múltiples peers
- `SendToServer()` para el flujo cliente-servidor
- `RegisterHandler()` y `ClearAllHandlers()` para controlar el ciclo de vida del despacho

Lo importante no es solo que existan estos métodos, sino la lógica con la que quedaron delimitados. Cada operación ahora tiene una responsabilidad más clara y una frontera más predecible.

### 1.6 Limpieza de estado y cierre de sesión

Otro aspecto técnico abordado fue la limpieza del estado de red. El problema de una capa de red mal desacoplada no aparece solo en el envío y recepción, sino en lo que queda colgando tras reconexiones, desconexiones o reemplazos de escena.

Por ello, la lógica de teardown fue tratada de forma explícita:

- limpieza de handlers registrados
- reinicio de estado de sesión
- liberación de peers/hosts de ENet
- separación entre “terminar el uso local” y “destruir por completo el contexto de red”

Esto mejora mantenibilidad y disminuye estados zombies dentro del motor.

## 2. Mejoras al sistema de entrada

### 2.1 Normalización de estados de entrada

El `InputManager` ya trabajaba con el patrón de estados `KeyDown`, `KeyUp` y `KeyRelease` para teclado y mouse. Durante la sesión, ese mismo modelo se extendió al control de joystick/gamepad para que los consumidores del motor pudieran consultar estados transitorios y sostenidos con la misma semántica.

Esto unifica el contrato de entrada del motor:

- una tecla puede consultarse como presionada en el frame actual, soltada o sostenida
- un botón de mando puede consultarse con exactamente la misma semántica

El valor técnico de esto es que el código de gameplay o UI no necesita conocer la fuente física de la entrada; consume una interfaz uniforme.

### 2.2 Lógica interna de `InputManager::update()`

La lógica de actualización del input quedó estructurada en varias fases:

1. Reinicio de estados transitorios (`keyDownState`, `keyUpState`, `mouseButtonDownState`, `mouseButtonUpState`, `joystickButtonDownState`, `joystickButtonUpState`).
2. Recorrido del teclado con `glfwGetKey()` para reconstruir transiciones de presionado/sostenido/soltado.
3. Recorrido del mouse con `glfwGetMouseButton()` con la misma lógica de transición.
4. Detección de joystick o gamepad activo.
5. Polling de ejes y botones analógicos/digitales.
6. Actualización de posición normalizada del mouse dentro del viewport.

Este orden no es accidental. Los estados transitorios deben resetearse antes del polling; de lo contrario, un `KeyDown` podría persistir más de un frame y romper la semántica de entrada discreta.

### 2.3 Soporte para gamepad mapeado y joystick genérico

En Linux no todos los dispositivos expuestos por GLFW aparecen como gamepad válido para `glfwGetGamepadState()`. Algunos solo aparecen como joystick genérico, especialmente en contextos como Steam Deck, Steam Input o entornos donde el runtime media la entrada.

Para cubrir ambos casos, se implementó una lógica de dos rutas:

- Ruta preferida: `glfwJoystickIsGamepad()` seguido de `glfwGetGamepadState()`.
- Ruta de respaldo: `glfwGetJoystickAxes()` y `glfwGetJoystickButtons()`.

Internamente esto se apoya en:

- `activeGamepadId`
- `activeJoystickUsesGamepadApi`
- `joystickButtonState`
- `joystickButtonDownState`
- `joystickButtonUpState`
- `joystickAxisState`

La lógica del fallback genérico hace un mapeo por convención de índices de ejes y botones hacia los identificadores equivalentes de GLFW gamepad. No es una abstracción perfecta para todos los dispositivos del mercado, pero mejora significativamente la robustez del motor frente a dispositivos no mapeados formalmente.

### 2.4 Dead-zone para ejes analógicos

Los valores analógicos se filtran con una dead-zone. La razón técnica es evitar drift del stick cuando el hardware no retorna exactamente a cero.

La lógica aplicada es:

- leer el valor del eje
- si `abs(valor) < umbral`, normalizarlo a `0.0f`
- guardar el resultado en `joystickAxisState`

Esto evita movimientos espurios y hace que el consumidor del motor pueda leer el eje sin volver a filtrar ruido en cada sistema.

### 2.5 Cola de caracteres con GLFW

Uno de los cambios más importantes para Linux fue separar “tecla” de “carácter”. Antes, el texto se reconstruía a partir de rangos de teclas (`A-Z`, `0-9`, símbolos puntuales). Eso no representa correctamente el texto real emitido por el sistema operativo.

Para corregirlo, el `InputManager` fue ampliado con:

- `std::deque<unsigned int> queuedCharacterInput`
- `glfwSetCharCallback(...)`
- `PollCharacterInput(unsigned int &character)`
- `ClearCharacterInput()`

La lógica es la siguiente:

1. GLFW emite un codepoint por callback de texto.
2. El callback inserta ese codepoint en `queuedCharacterInput`.
3. Los elementos UI que aceptan texto leen la cola en su `update()`.
4. Los caracteres se insertan en el buffer del widget de forma secuencial.

Este cambio es especialmente relevante para Linux, teclados internacionales y entornos con teclado virtual.

## 3. Ajustes técnicos al subsistema UI

### 3.1 Corrección semántica de visibilidad en `UIElement`

Se corrigió la lógica base de interacción para que un elemento invisible no responda a hover. La modificación se aplicó directamente en `UIElement::MouseHover()` con una salida temprana cuando `visible == false`.

Además, `UIManager::update(float deltaTime)` fue ajustado para omitir elementos no visibles antes de actualizarlos o incluirlos en el cálculo de `MouseHoverAny`.

La lógica previa era defectuosa porque mezclaba dos conceptos distintos:

- visibilidad visual
- participación en el sistema de interacción

Después del ajuste, ambos conceptos quedan alineados, que es lo correcto para un sistema UI retenido.

### 3.2 Revisión técnica de `UITextField`

El `UITextField` recibió una revisión estructural importante. Antes se comportaba como un buffer simple con entrada lineal. Después de los cambios, el widget quedó más cerca de un control editable real.

Las variables de estado relevantes son:

- `text`
- `cursorIndex`
- `focused`
- `cursorBlinkTimer`
- `cursorVisible`
- `backspaceHoldTimer`
- `backspaceRepeatTimer`

Los métodos clave trabajados fueron:

- `update(float deltaTime)`
- `HandleInput(float deltaTime)`
- `InsertCharacter(char character)`
- `RemoveCharacterBeforeCursor()`
- `ResetCursorBlink()`
- `NotifyTextChanged()`
- `GetCursorOffsetX() const`

### 3.3 Lógica de foco y edición

La lógica final del widget se puede resumir así:

1. Un clic determina foco mediante `MouseHover()`.
2. Si el control gana foco, se reinicia el blink del cursor y se limpia la cola de caracteres residuales.
3. Si pierde foco, también se limpia el buffer pendiente de texto y se reinician timers asociados a backspace.
4. Si está enfocado, procesa:
	 - backspace discreto y repetido
	 - flechas izquierda/derecha
	 - enter para submit
	 - escape para perder foco
	 - caracteres imprimibles desde la cola del `InputManager`

La parte técnica más importante aquí es que el widget ya no deduce texto desde teclas físicas, sino que consume la abstracción correcta de caracteres emitidos por la plataforma.

### 3.4 Inserción en posición de cursor

La lógica de edición fue corregida para que `InsertCharacter()` no agregue siempre al final. En su lugar, usa `cursorIndex` para insertar en la posición actual.

La operación interna es conceptualmente:

```cpp
text.insert(text.begin() + cursorIndex, character);
+cursorIndex;
```

Esto también obligó a que `RemoveCharacterBeforeCursor()` elimine relativo al cursor y no relativo al final del string.

Como resultado, el widget soporta navegación horizontal y edición localizada.

### 3.5 Repetición de backspace

La eliminación repetida se implementó con dos temporizadores:

- `backspaceHoldTimer`
- `backspaceRepeatTimer`

La lógica aplicada es:

1. Si backspace entra en `KeyDown`, se elimina un carácter inmediatamente.
2. Si backspace permanece en `KeyRelease`, se acumula `backspaceHoldTimer`.
3. Al superar el umbral de espera, se activa repetición periódica mediante `backspaceRepeatTimer`.

Esto evita borrado instantáneo excesivo y replica un comportamiento conocido de controles de texto convencionales.

### 3.6 Render del cursor y color de foco

`UITextField::draw()` fue ampliado para que el cursor visual se dibuje según la posición real del índice de inserción. El desplazamiento horizontal se calcula con `GetCursorOffsetX()`, que usa `ResourceManager::MeasureText(...)` sobre el substring desde el inicio hasta `cursorIndex`.

Esto hace que el cursor no sea una decoración estática, sino una representación directa del estado interno del campo.

Además, `GetCurrentTextColor()` permite cambiar color de texto al tomar foco, reforzando visualmente el estado de edición.

## 4. Corrección del pipeline de render UI en modo 3D

### 4.1 Problema detectado

Durante la sesión apareció un artefacto visual en UI cuando el motor trabajaba en configuración 3D. Aunque inicialmente parecía un fallo del cursor del `UITextField`, el problema real estaba más abajo, dentro del `ResourceManager`.

### 4.2 Causa raíz

En la ruta de `RenderRectangle(...)` usada para dibujar rectángulos UI en contexto 3D, se estaba subiendo un buffer de vértices 2D (`glm::vec2`) hacia un VBO configurado con layout de `glm::vec3`.

Ese desalineamiento de formato provoca corrupción geométrica porque:

- el atributo de posición espera tres componentes por vértice
- el buffer proveía solo dos
- el stride y la lectura en GPU dejaban memoria interpretada de forma inválida

El resultado visible fueron líneas diagonales y bordes distorsionados en elementos UI.

### 4.3 Corrección aplicada

La solución se hizo en la raíz del problema: la subida de vértices en `ResourceManager`, no en el widget afectado.

La lógica corregida consiste en generar y subir vértices `glm::vec3` cuando la ruta de render en 3D utiliza un VBO configurado para ese formato. De esta forma, el contrato entre CPU y GPU vuelve a ser consistente.

La relevancia técnica de este arreglo es que no corrige solo un cursor, sino toda una clase de errores del pipeline UI en 3D.

## 5. Ajustes al administrador de cámara

Durante la sesión se completó la lógica de helpers de foco 3D dentro de `CameraManager`, en particular para escenarios donde el motor necesita fijar explícitamente una posición de cámara y un punto de observación sin delegar el control a un camera controller libre.

La idea técnica detrás de esto es separar dos casos de uso:

- cámara controlada dinámicamente por input
- cámara fijada por lógica superior mediante `SetFocusPosition(...)`

Este cambio mejora la utilidad del manager como infraestructura general y no solo como wrapper de controladores interactivos.

## 6. Adaptación del build para Linux y reorganización de salidas

### 6.1 Reorganización de `CMakeLists.txt`

El `CMakeLists.txt` principal fue refactorizado para separar explícitamente la configuración de Windows y Linux, no solo a nivel de compilación, sino también a nivel de salida runtime y empaquetado.

La lógica resultante quedó aproximadamente así:

- En Windows:
	- se conserva el recurso de ícono
	- se enlazan librerías estáticas locales del proyecto
	- se copian DLLs de runtime cuando corresponde
	- se agregan librerías del sistema como `ws2_32`, `mswsock`, `winmm`, `ole32`, `uuid`
    	- la salida se emite dentro de `bin/windows/<configuracion>`

- En Linux:
	- se usa `find_package(...)` para dependencias del sistema
	- se usa `pkg_check_modules(...)` para paquetes como GLFW
	- se evita la lógica de `.exe` y DLL propia de Windows
	- la salida se emite dentro de `bin/linux/<configuracion>`

Esto elimina supuestos que antes solo eran válidos en el entorno Windows del repositorio.

### 6.2 Reestructuración de la jerarquía `bin/`

Uno de los cambios más visibles del sistema de build fue la separación física de artefactos por plataforma y configuración. En lugar de escribir todos los binarios en una única carpeta, el motor ahora genera una estructura explícita:

- `bin/windows/Release`
- `bin/windows/Debug`
- `bin/windows/RelWithDebInfo`
- `bin/windows/MinSizeRel`
- `bin/linux/Release`
- `bin/linux/Debug`
- `bin/linux/RelWithDebInfo`
- `bin/linux/MinSizeRel`

Esta reestructuración tiene una motivación técnica clara:

- evita mezclar artefactos incompatibles de Windows y Linux en un mismo directorio
- facilita empaquetado y distribución por plataforma
- evita que DLLs de Windows queden junto a binarios ELF de Linux
- simplifica tareas post-build, ya que cada plataforma opera sobre su propio runtime directory

La salida ya no se considera solamente una carpeta de compilación, sino una carpeta distribuible por plataforma.

### 6.3 Dependencias del sistema en Linux

La ruta Linux fue alineada con dependencias reales del entorno:

- OpenGL
- Freetype
- Threads
- ZLIB
- Assimp
- GLFW

El beneficio técnico de este cambio es que la compilación deja de depender de librerías precompiladas empaquetadas manualmente para Windows, y pasa a integrarse con el gestor de paquetes y el toolchain del sistema Linux.

### 6.4 Problema de ENet y resolución de símbolos

El punto más delicado del build Linux fue ENet. El proyecto incluía en el repositorio una implementación bundled de ENet, pero en Linux estaba intentando enlazar contra la librería del sistema. Eso creó una incompatibilidad entre:

- el header usado al compilar
- la librería real usada al linkear

El síntoma concreto fue un error del tipo:

```text
undefined reference to enet_address_set_host_old
```

La corrección consistió en usar una sola fuente de verdad para ENet dentro de Linux: compilar la implementación incluida en el repositorio mediante un archivo puente (`src/enet_impl.c`) que incluye `enet_imp.h`.

Eso garantiza:

- consistencia entre símbolos declarados y definidos
- independencia respecto a la versión instalada del paquete del sistema
- reproducibilidad del subsistema de red entre plataformas

### 6.5 Correcciones por case sensitivity

Linux expuso también errores de include que Windows toleraba por su comportamiento case-insensitive. Se corrigieron rutas de cabeceras cuya capitalización no coincidía exactamente con el nombre real del archivo.

Aunque el cambio parece menor, técnicamente es crítico porque un proyecto no es realmente portable si compila solo por tolerancia del sistema de archivos.

### 6.6 Ajuste del target `run`

La lógica del target de ejecución fue corregida para utilizar `$<TARGET_FILE:${PROJECT_NAME}>` en lugar de asumir un nombre de ejecutable con extensión `.exe`.

Esto es importante porque:

- en Windows el ejecutable termina en `.exe`
- en Linux no
- CMake ya conoce la ruta exacta del target compilado

Usar el path resuelto por CMake elimina bifurcaciones innecesarias y mejora portabilidad del flujo de ejecución.

### 6.7 Empaquetado post-build por plataforma

Después de la reorganización de carpetas, el sistema de build fue extendido para que cada salida de runtime incluya no solo el ejecutable, sino también los recursos y archivos auxiliares necesarios para iniciar el proyecto.

La lógica post-build ahora realiza, según la plataforma:

- copia de `assets/`
- copia de `imgui.ini`
- copia de `json_demo_save.json`
- creación de carpeta `logs/`

Adicionalmente:

- En Windows se copian las DLLs de runtime detectadas, incluyendo las asociadas al toolchain MinGW y las existentes dentro de `dll/`.
- En Linux se crea una carpeta `lib/` y se copian bibliotecas compartidas `.so` necesarias para ejecutar el binario fuera de la máquina de compilación.

Con esto, el resultado de cada build deja de ser únicamente un binario enlazado y pasa a ser un paquete runnable por plataforma.

### 6.8 Empaquetado de bibliotecas compartidas en Linux

Para Linux se añadió un flujo específico de bundling mediante scripts CMake auxiliares:

- `cmake/BundleLinuxDeps.cmake`
- `cmake/WriteLinuxLauncher.cmake`

La lógica de `BundleLinuxDeps.cmake` es:

1. ejecutar `ldd` sobre el binario generado
2. parsear las bibliotecas resueltas
3. filtrar bibliotecas base del sistema que no conviene bundlear directamente, como `libc.so.6`, `libm.so.6` o el loader dinámico
4. copiar las bibliotecas compartidas relevantes al directorio local `lib/`
5. copiar tanto el nombre real como el alias necesario cuando existen versiones tipo `libfoo.so.X -> libfoo.so.X.Y.Z`

Este detalle es importante porque, en Linux, copiar solo el symlink no basta; el archivo real al que apunta también debe estar presente.

### 6.9 `RPATH` y launcher Linux

Además del bundling de bibliotecas, se añadió configuración de `RPATH` con `$ORIGIN/lib` para que el ejecutable pueda buscar bibliotecas junto a su propia carpeta.

Sin embargo, en Linux las dependencias transitivas no siempre se resuelven únicamente con el `RPATH` del ejecutable. Por esa razón, se agregó además un launcher:

- `launch.sh`

La lógica de este launcher es:

1. resolver el directorio actual del paquete
2. exportar `LD_LIBRARY_PATH` apuntando a `./lib`
3. ejecutar el binario real usando ese entorno

Desde el punto de vista técnico, esto hace que la carpeta `bin/linux/<configuracion>` sea mucho más portable entre máquinas Linux, porque obliga al runtime linker a preferir las bibliotecas bundleadas con el proyecto.

### 6.10 Resultado final de cada salida runtime

Con la estructura actual, cada carpeta de salida por plataforma contiene una composición parecida a esta:

- ejecutable principal
- `assets/`
- `imgui.ini`
- `json_demo_save.json`
- `logs/`

Y además:

- En Windows: DLLs requeridas para el arranque del ejecutable.
- En Linux: carpeta `lib/` con bibliotecas `.so` bundleadas y `launch.sh` como punto de entrada recomendado.

Esto no implica un binario completamente estático ni elimina todas las dependencias base del sistema operativo, pero sí reduce sustancialmente la necesidad de instalar manualmente librerías de terceros en la máquina destino.

## 7. Validación técnica realizada

Los cambios no quedaron solo a nivel de edición estática. Se validaron mediante compilación real en ambos entornos:

- build nativo en Windows exitoso
- build en Ubuntu sobre WSL exitoso
- eliminación del error de link de ENet en Linux
- generación correcta de salida en `bin/windows/Release`
- generación correcta de salida en `bin/linux/Release`
- creación de carpeta `lib/` para Linux
- generación del launcher `launch.sh` para Linux
- validación de dependencias del binario mediante `ldd`

Esto confirma que el trabajo sobre el sistema de build y las dependencias no fue teórico, sino verificado contra un toolchain Linux funcional y contra una estructura de salida realmente distribuible por plataforma.

## 8. Estado técnico resultante del motor

Tras los cambios realizados en la sesión, CosmicEngine queda mejor posicionado en varios frentes estructurales:

- La capa de red es más modular y más reutilizable.
- La lógica de mensajes y handlers está mejor separada del transporte.
- El sistema de entrada ya no se limita a teclado y mouse; soporta gamepad y joystick genérico.
- La entrada de texto utiliza la abstracción correcta de caracteres emitidos por GLFW.
- `UITextField` dispone de cursor real, inserción localizada, backspace repetido y foco visual.
- Los elementos UI ocultos ya no siguen interactuando con el mouse.
- El render de UI en modo 3D respeta el layout real del buffer de vértices.
- El sistema de build fue adaptado a Linux con resolución correcta de dependencias.
- ENet quedó unificado para evitar desajustes entre headers y símbolos enlazados.
- La carpeta `bin/` quedó separada por plataforma y configuración.
- Cada salida runtime ahora empaqueta recursos, archivos auxiliares y dependencias necesarias para el arranque.
- En Linux se añadió un launcher explícito para resolver bibliotecas bundleadas desde la propia carpeta del proyecto.

