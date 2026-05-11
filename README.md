# WandEngine

## Fecha efectiva

**21 de abril de 2025**

## Descripción general

En esta etapa el cambio dominante ya no está en agregar subsistemas al core, sino en normalizar su contrato público. El framework reorganiza sus tipos base alrededor de nombres más generales, introduce una configuración explícita de proyecto para elegir entre modo 2D y 3D en tiempo de compilación, y desacopla parte del bootstrap del motor mediante callbacks y constantes globales de inicialización.

La evidencia visible de esta versión se concentra sobre todo en tres decisiones: `GameObject`, `GameScene` y `GameTimer` desaparecen como nombres públicos y son sustituidos por `Object`, `Scene` y `Timer`; `Configurations.hpp` pasa a controlar el perfil activo del engine y condiciona la forma de varios managers y modelos; y `GameManager` deja de codificar ciertos callbacks como detalle interno fijo para exponerlos mediante `std::function`.

Con ello el proyecto no abandona la expansión del 12 de abril, pero sí la reordena. El framework queda menos atado a una semántica de demo o de implementación histórica, y más cerca de una API de engine reusable con perfiles de compilación definidos.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- homogeneizar la nomenclatura del núcleo para que sus clases base representen conceptos de engine y no variantes heredadas del prefijo `Game`;
- introducir una puerta de configuración única para decidir el perfil 2D o 3D del proyecto;
- desacoplar el arranque y ciertos eventos del runtime para que el framework sea más configurable desde afuera.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `include/WandEngine/Models/Object/Object.hpp`, `include/WandEngine/Models/Scene/Scene.hpp` y `include/WandEngine/Models/Timer/Timer.hpp`, donde el contrato base del runtime se reexpresa con nombres más generales y una organización nueva por dominio.
- `include/WandEngine/Utils/Configurations.hpp`, que aparece como archivo central de selección de configuración del proyecto y define dimensiones iniciales y perfil de render.
- `include/WandEngine/Managers/Object/ObjectManager.hpp`, `include/WandEngine/Managers/Scene/SceneManager.hpp` y `include/WandEngine/Managers/Timer/TimerManager.hpp`, donde los managers se ajustan a los nuevos nombres base y a la bifurcación 2D/3D.
- `include/WandEngine/Managers/GameManager.hpp`, donde el manager principal incorpora callbacks configurables con `std::function` y se apoya en la nueva configuración global.
- `src/main.cpp`, únicamente como evidencia de integración del framework, donde el arranque pasa a usar constantes de `Configurations.hpp`, logging explícito y una escena 3D de entrada.

## Arquitectura o sistemas principales

### Normalización del modelo base del runtime

El cambio técnico más fuerte de esta etapa está en la sustitución de las clases base con prefijo `Game` por nombres más neutrales. `Object`, `Scene` y `Timer` pasan a ser las referencias principales del framework para entidad, escena y temporizador.

Esto no es solo un cambio cosmético de nombres. También reordena el árbol público del engine y deja más clara la intención de uso de cada tipo: ya no se presentan como piezas ligadas a una implementación concreta del juego, sino como abstracciones base del framework.

El resultado es una API más limpia para construir sobre ella, aunque el propio diff deja ver que la migración todavía está en curso y conserva algunas referencias heredadas en headers auxiliares.

### Configuración de proyecto como puerta de compilación

`Configurations.hpp` se vuelve una pieza estructural del motor. En lugar de asumir implícitamente un perfil de ejecución, el framework define macros de configuración para elegir entre `GAME_2D_CONFIGURATION` y `GAME_3D_CONFIGURATION`, además de constantes para tamaño inicial de ventana y base de render.

La implicación aquí es fuerte porque varios contratos pasan a depender de esa selección. `Object`, por ejemplo, cambia sus propiedades públicas y protegidas según el modo activo: vectores 2D con `layerId` y rotación escalar para un perfil, o vectores 3D y rotaciones vectoriales para el otro.

Eso convierte la configuración del proyecto en una decisión explícita del framework y no en una acumulación de supuestos dispersos.

### Managers adaptados a perfiles 2D y 3D

`ObjectManager` hace visible esa misma bifurcación con una API condicionada por compilación. En modo 2D mantiene búsquedas por capa, área o posición del mouse; en modo 3D abre rutas como búsquedas por volumen o raycast de vista.

La importancia de este punto está en que la arquitectura deja de fingir una neutralidad total cuando en realidad el dominio cambia entre 2D y 3D. En lugar de esconder esa diferencia, la hace parte formal del contrato del manager.

`SceneManager` y `TimerManager` también se ajustan al nuevo vocabulario del core, reforzando la idea de un runtime más consistente alrededor de `Scene` y `Timer` como tipos base.

### GameManager más desacoplado del detalle de ventana

`GameManager` también cambia de tono. Sigue siendo el punto de entrada del ciclo principal, pero ahora admite callbacks configurables de framebuffer y drop mediante `std::function`, en lugar de tratar esos eventos como rutas internas rígidas.

Eso reduce acoplamiento con la implementación directa de GLFW y permite que el usuario del framework extienda o reemplace parte del comportamiento del bootstrap sin intervenir el núcleo del manager.

Además, el uso conjunto con `Configurations.hpp` mueve la inicialización de pantalla y aspecto a un esquema más declarativo y menos hardcodeado dentro del arranque.

### Bootstrap del framework más explícito

Aunque no conviene documentar la demo como eje del cambio, `src/main.cpp` sí confirma un ajuste propio del framework: el arranque se apoya ahora en `Configurations.hpp`, usa logging explícito y selecciona la escena inicial desde una interfaz más limpia.

Eso refuerza la lectura general de esta fecha: el motor no solo renombra tipos, sino que también intenta hacer más predecible y configurable el punto de entrada para proyectos que lo consuman.

### Migración en curso, no sustitución completamente cerrada

El árbol público deja ver que esta normalización todavía no está completada del todo. Persisten referencias heredadas como macros antiguas en `Definitions.hpp` o forward declarations con nombres previos en algunos managers.

Eso conviene dejarlo explícito porque describe mejor la realidad del diff: el 21 de abril no es una limpieza final del framework, sino una fase fuerte de transición hacia una API más estable y genérica.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en tres sentidos:

- el proyecto puede declarar desde un único punto si compila contra un perfil 2D o 3D;
- los tipos base del runtime quedan menos acoplados al vocabulario histórico de la reconstrucción;
- el bootstrap del motor expone más puntos de extensión para integrar callbacks y parámetros de inicialización desde afuera.

Con ello dejé una base de engine más configurable que la del 12 de abril. La mejora principal de esta etapa no está en añadir render, UI o persistencia, sino en ordenar cómo el framework se presenta, se configura y se consume.

## Dependencias externas visibles

No aparece un cambio principal de librerías externas en esta etapa. El cambio está en cómo el framework organiza su relación con ellas:

- GLFW sigue siendo la base de la ventana y eventos, pero `GameManager` encapsula mejor sus callbacks;
- OpenGL y el resto del stack gráfico permanecen como infraestructura ya existente, sin ser el eje dominante de esta fecha;
- `sqlite3` y otros subsistemas del core continúan presentes, pero aquí operan como continuidad del estado previo y no como la novedad principal.

## Resumen técnico de la versión

La etapa efectiva al **21 de abril de 2025** queda delimitada por estos movimientos:

- renombré y reorganicé el modelo base del runtime alrededor de `Object`, `Scene` y `Timer`;
- introduje `Configurations.hpp` como punto único para definir perfil 2D/3D y constantes principales de inicialización;
- adapté managers como `ObjectManager`, `SceneManager` y `TimerManager` al nuevo vocabulario del framework y a la bifurcación por configuración;
- desacoplé `GameManager` mediante callbacks configurables con `std::function` y un arranque más declarativo;
- alineé el bootstrap del engine con logging explícito y parámetros de configuración centralizados;
- dejé visible que la migración todavía está en curso, porque sobreviven referencias heredadas del esquema anterior en parte del árbol público.

Con esta etapa dejé el framework en una transición importante hacia una API más genérica y reusable: menos peso del legado nominal, más configuración explícita del proyecto y un núcleo mejor preparado para separar perfiles 2D y 3D.