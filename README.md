# WandEngine

## Fecha efectiva

**12 de abril de 2025**

## Descripción general

En esta etapa el cambio dominante ya no es la depuración del core, sino su recomposición sobre una API más ambiciosa y coherente. El framework vuelve a crecer alrededor de un `ResourceManager` mucho más expresivo, reincorpora texto y persistencia declarativa al núcleo, y formaliza una capa de UI propia integrada al ciclo principal del engine.

La evidencia visible de esta versión se concentra sobre todo en cuatro decisiones: `ResourceManager` pasa a centralizar sprites, formas, texto y buffers con soporte de múltiples vistas; `Shader` recupera helpers de alto nivel y unifica la proyección mediante `ViewType`; `UIManager`, `UIButton` y `UIText` vuelven a ser parte explícita del framework; y `DataBaseManager` junto con `GameObject` reabren la ruta de serialización por clase dentro del core.

Con ello el proyecto no solo recupera capacidad perdida respecto a días anteriores, sino que reorganiza esa capacidad bajo contratos públicos más consistentes. El framework queda otra vez más amplio, pero también más alineado entre render, UI, runtime y persistencia.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- consolidar un pipeline gráfico único capaz de atender render 2D, UI y distintas proyecciones desde una misma base de recursos;
- reincorporar subsistemas transversales del framework, especialmente texto, UI y persistencia estructurada;
- estandarizar los managers y contratos base del runtime para que el uso del engine sea más uniforme.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `include/WandEngine/Managers/Resource/ResourceManager.*`, donde reaparece una infraestructura amplia con VAO estáticos, buffers dinámicos, fuentes tipográficas, medición de texto y helpers de render sobre múltiples vistas.
- `include/WandEngine/Models/Shader.*`, donde vuelve una API de mayor nivel con `ViewType`, `SetModel()`, `SetProjection()` y un uso más expresivo de uniforms y compilación.
- `include/WandEngine/Models/TextFont/`, que regresa al árbol del framework para sostener la carga de glifos y el render de texto desde el propio engine.
- `include/WandEngine/Managers/UI/` y `include/WandEngine/Models/UIElements/`, donde se formaliza otra vez una capa de UI con manager dedicado y elementos como `UIButton` y `UIText`.
- `include/WandEngine/Managers/DataBase/` y `include/WandEngine/Models/GameObject.*`, donde reaparecen la serialización por clase, `GetAllValues()`, `Reset()` y el contrato para reconstrucción de objetos desde persistencia.
- `include/WandEngine/Managers/GameManager.*` y `include/WandEngine/Interfaces/Definitions.hpp`, donde se estabiliza la ergonomía del framework con singleton managers, drag-and-drop, aspecto base y aliases globales para UI y base de datos.

## Arquitectura o sistemas principales

### ResourceManager como hub de render del framework

El cambio técnico más fuerte de esta etapa está en `ResourceManager`. El manager vuelve a convertirse en el centro real del pipeline gráfico del engine y deja de limitarse a texturas y shaders básicos. En esta versión concentra:

- VAO estáticos y buffers dinámicos diferenciados;
- render de sprites, spritesheets y primitivas geométricas;
- carga y render de texto mediante `TextFont`;
- helpers como `MeasureText()` y `RenderText()`;
- selección de vista mediante `ViewType` para reutilizar la misma infraestructura entre mundo, proyección y UI.

Eso redefine el alcance del subsistema: ya no es solo un repositorio de recursos, sino una capa integrada de render con semántica propia del framework.

### Proyección unificada con ViewType

`Shader` recupera una API más rica y reaparece `ViewType` como enum central del pipeline. Con `SetProjection()` y `SetModel()`, el framework vuelve a encapsular transformaciones y elección de matrices dentro del propio shader wrapper, en lugar de dejar toda esa responsabilidad dispersa por fuera.

La implicación arquitectónica es importante porque ordena el render alrededor de una misma convención de vistas. Un sprite, una forma o un texto pueden pasar por el mismo esquema de proyección sin duplicar lógica por subsistema.

### Texto y UI otra vez dentro del core

La vuelta de `TextFont` no llega aislada. Está acompañada por la reaparición de `UIManager`, `UIButton` y `UIText`, lo que convierte al texto en un servicio transversal y no solo en una utilidad auxiliar. El framework vuelve a ofrecer una capa de interfaz propia, con actualización y dibujo integrados al ciclo de escena.

En términos de diseño, esto vuelve a acercar el engine a una plataforma más completa para herramientas y HUDs. La UI deja de ser responsabilidad externa y pasa a formar parte explícita del contrato del motor.

### Persistencia declarativa y GameObject extendido

`DataBaseManager` también regresa con un enfoque estructurado: registro de serialización por clase, columnas declaradas, callbacks de reconstrucción y rutas de guardado y carga por tipo. Eso vuelve a insertar la persistencia en el corazón del framework.

Para sostenerlo, `GameObject` retoma un contrato más amplio. Reaparecen `GetAllValues()`, `Reset()` y `Draw() const`, además de una semántica más preparada para clonación, referencias y restauración de estado. El modelo base vuelve a exponer la información que la persistencia necesita sin delegarla a lógica externa al objeto.

Con ello el runtime recupera una ambición que en la etapa previa se había retirado: el motor vuelve a describir, guardar y reconstruir parte de su estado como responsabilidad interna.

### Managers más uniformes y framework más ergonómico

El ensanchamiento del core también viene acompañado por una estandarización de managers y atajos públicos. `GameManager` incorpora soporte para archivos soltados sobre la ventana y control explícito del aspecto base, mientras `Definitions.hpp` vuelve a exponer aliases para UI y base de datos junto a los demás managers del engine.

No es el cambio más vistoso, pero sí uno importante para uso diario: el framework queda más homogéneo en cómo inicializa, centraliza y expone sus subsistemas.

### Mesh y Model como soporte, no como eje dominante

`Mesh` y `Model` siguen presentes dentro del stack del engine y forman parte del soporte general para geometría y carga de assets 3D. Sin embargo, en esta fecha no son el centro del cambio arquitectónico. El peso real del delta está en la recomposición de recursos, vistas, UI, texto y persistencia.

Eso conviene dejarlo explícito para no sobredocumentar un área que aquí actúa más como continuidad del framework que como novedad dominante.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en tres sentidos:

- el render vuelve a apoyarse en una infraestructura unificada capaz de atender mundo, proyección y UI bajo una misma convención;
- la capa de interfaz y texto ya puede vivir dentro del motor sin depender de soluciones externas ad hoc;
- la persistencia vuelve a disponer de contratos explícitos entre `DataBaseManager` y `GameObject` para guardar y reconstruir objetos del runtime.

Con ello dejé una base de engine bastante más completa que la del 6 de abril. La mejora principal de esta etapa no está en un subsistema aislado, sino en cómo varias capas del framework vuelven a conectarse entre sí bajo una API más consistente.

## Dependencias externas visibles

No aparece un cambio de stack principal, pero sí una ampliación clara de cómo el framework usa dependencias ya presentes:

- OpenGL sigue siendo la base del render, ahora con una envoltura más rica en `ResourceManager` y `Shader`;
- FreeType vuelve a tener presencia directa en el core a través de `TextFont` y del render de texto;
- `sqlite3` vuelve a sostener una capa propia del framework mediante `DataBaseManager` y la serialización declarativa por clase;
- Assimp y el stack de modelos siguen integrados, aunque en esta fecha actúan como soporte y no como eje principal del cambio.

## Resumen técnico de la versión

La etapa efectiva al **12 de abril de 2025** queda delimitada por estos movimientos:

- reconstruí `ResourceManager` como hub central de sprites, formas, texto y buffers con soporte de múltiples vistas;
- restauré una API de shader más rica con `ViewType`, `SetModel()` y `SetProjection()`;
- reincorporé `TextFont` y el render de texto al núcleo del framework;
- formalicé otra vez una capa de UI con `UIManager`, `UIButton` y `UIText` integrada al ciclo del motor;
- reintroduje `DataBaseManager` y devolví a `GameObject` los contratos necesarios para serialización y restauración de estado;
- amplié la ergonomía del engine con drag-and-drop, aspecto base y aliases globales para subsistemas que vuelven a ser parte del core.

Con esta etapa dejé el framework en una versión claramente más completa y más integrada: un core otra vez amplio, con más servicios internos y con una arquitectura pública mejor alineada entre render, UI, runtime y persistencia.