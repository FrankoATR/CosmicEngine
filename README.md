# WandEngine

## Fecha efectiva

**6 de abril de 2025**

## Descripción general

En esta etapa el cambio dominante deja de ser la expansión del framework y pasa a ser una depuración fuerte del core. El motor retira varias capas experimentales que habían aparecido poco antes, especialmente la persistencia estructurada con base de datos y la ruta dedicada de texto, y vuelve a una API más directa en recursos, shaders, texturas y modelo base de objetos.

La evidencia visible de esta versión se concentra sobre todo en tres decisiones: desaparecen por completo `DataBaseManager` y `TextFont`; `ResourceManager`, `Shader` y `GameTexture2D` revierten hacia una interfaz más manual y menos abstracta; y `GameObject` junto con `GameScene` recortan hooks que habían ampliado el contrato del runtime en fechas anteriores.

Con ello el proyecto no gana un subsistema nuevo, pero sí reduce complejidad interna. El framework queda más pequeño, más directo y menos cargado de capas todavía no estabilizadas.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- retirar subsistemas experimentales que todavía no estaban consolidados dentro del motor;
- simplificar la API gráfica de recursos, shaders y texturas hacia un uso más explícito y manual;
- reducir el contrato base de objetos y escenas para volver a una semántica más contenida del runtime.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `include/WandEngine/Managers/DataBase/`, que desaparece del árbol del framework, eliminando la capa de serialización y persistencia declarativa basada en SQLite.
- `include/WandEngine/Models/TextFont/`, que también desaparece, retirando la ruta de render de texto apoyada en FreeType desde el propio core.
- `include/WandEngine/Managers/Resource/ResourceManager.*`, donde se abandona la separación entre recursos estáticos y dinámicos, se eliminan las claves internas `WAND_*` y se vuelve a una interfaz de recursos más simple.
- `include/WandEngine/Models/Shader.*` y `include/WandEngine/Models/Texture/GameTexture2D.*`, donde se revierten helpers de alto nivel y se recupera un manejo más manual de compilación, uniforms y carga de texturas.
- `include/WandEngine/Models/GameObject.*` y `include/WandEngine/Models/GameScene.*`, donde se recortan métodos como `GetAllValues()`, `Reset()` y parte del endurecimiento previo del contrato base.
- `include/WandEngine/Interfaces/Definitions.hpp`, donde desaparecen aliases visibles ligados a UI y base de datos, reduciendo la superficie de atajos globales del framework.

## Arquitectura o sistemas principales

### Retirada de persistencia estructurada

El cambio técnico más fuerte de esta etapa es la eliminación completa de `DataBaseManager`. Con ello sale del framework toda la capa que intentaba formalizar:

- serialización por clase;
- constructores de reconstrucción desde filas SQL;
- guardado y carga por tipo del runtime;
- helpers de consulta acoplados a un flujo interno de persistencia.

La consecuencia arquitectónica es clara: el motor deja de tratar la persistencia como responsabilidad del core y vuelve a un estado donde esa preocupación queda fuera de su contrato principal.

### Retirada del render de texto como responsabilidad del core

La eliminación de `TextFont` va en la misma dirección. El framework abandona la ruta que había empezado a integrar glifos ASCII, texturas por carácter y render de texto dentro del propio `ResourceManager`.

No se sustituye por un sistema nuevo: simplemente se retira del núcleo. Eso reduce dependencias conceptuales dentro del motor y evita mantener una capa todavía inmadura junto al resto del pipeline gráfico.

### Reversión hacia un ResourceManager más simple

`ResourceManager` también cambia de tono. La versión anterior estaba caminando hacia una infraestructura más sofisticada, con registros separados para VAO estáticos y buffers dinámicos, claves internas reservadas y helpers genéricos de limpieza. En esta etapa, buena parte de esa sofisticación se desarma.

El resultado es un manager más directo, más manual y con menos estructura auxiliar. Se recuperan rutas como:

- `loadVAO` en lugar de variantes especializadas para geometría estática o dinámica;
- `loadShader`, `loadTexture` y `loadTextureSheet` con una semántica más inmediata;
- limpieza de recursos mediante bucles explícitos en lugar de utilidades genéricas como `ClearMap()`.

Eso reduce abstracción, pero también vuelve más visible y predecible el comportamiento de bajo nivel del subsistema.

### Shader y textura con menos helpers de alto nivel

La simplificación también alcanza a `Shader` y `GameTexture2D`. `Shader` vuelve a una forma más clásica: constructor vacío, compilación explícita y setters de uniforms con nomenclatura simple. Desaparecen helpers como `SetModel()`, `SetProjection()`, `Use()`, `EndUse()` y `GetID()` como parte del contrato refinado más reciente.

`GameTexture2D`, por su parte, deja de cargar imágenes desde el constructor y vuelve a un esquema donde otra capa carga datos y llama a `Generate(width, height, data)`. Eso desplaza responsabilidad fuera del objeto y reduce el encapsulamiento que se había intentado ganar en fechas anteriores.

La lectura técnica de ambos cambios es la misma: el framework prefiere aquí una API menos ambiciosa y más explícita en lugar de empujar más inteligencia a las abstracciones base.

### Contratos base más contenidos para objetos y escenas

`GameObject` y `GameScene` también se simplifican. Salen del contrato base varios puntos que ampliaban el alcance del runtime más allá de lo estrictamente necesario:

- `GetAllValues()` desaparece junto con la idea de serialización desde el modelo base;
- `Draw()` deja de ser `const`;
- `Reset()` sale del contrato base de `GameScene`;
- el orden de actualización de managers vuelve a priorizar primero subsistemas internos y luego la lógica de escena en un flujo más contenido.

Esto sugiere una decisión de diseño bastante clara: antes de seguir agregando responsabilidades al núcleo, el framework reduce su superficie a aquello que ya considera estable.

### Recorte de utilidades y ergonomía no esenciales

Varios detalles menores acompañan esa misma simplificación:

- desaparece `RS_MNX()` como helper inline de acceso al singleton de recursos;
- `CameraManager` elimina offsets en `SetFocusObject()` y vuelve a un enfoque más simple;
- `MusicManager` retira `SetPosition()`;
- `GameManager` elimina la ruta de `DropCallback()` y el manejo de archivos soltados sobre la ventana.

Cada ajuste por separado es pequeño, pero en conjunto refuerzan el mismo patrón: el framework reduce puntos de entrada accesorios y conserva una base más austera.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- el core del motor queda menos acoplado a subsistemas todavía experimentales como persistencia estructurada o render de texto;
- la capa gráfica vuelve a una interfaz más explícita, donde buena parte de la lógica queda en llamadas manuales y no en helpers de alto nivel.

Con ello dejé una base de engine más pequeña y más conservadora que la de días anteriores. La mejora principal de esta etapa no está en añadir capacidad, sino en retirar complejidad que todavía no estaba suficientemente asentada.

## Dependencias externas visibles

No aparecen dependencias nuevas visibles en esta etapa. Al contrario, el cambio principal es de contracción del uso interno de varias de ellas:

- `sqlite3` deja de sostener una capa propia del framework al eliminarse `DataBaseManager`;
- FreeType deja de estar representada en el core al desaparecer `TextFont`;
- OpenGL sigue siendo la base del render, pero con una envoltura menos rica y más manual en `Shader`, `GameTexture2D` y `ResourceManager`.

## Resumen técnico de la versión

La etapa efectiva al **6 de abril de 2025** queda delimitada por estos movimientos:

- eliminé `DataBaseManager` y retiré del core la persistencia declarativa de objetos;
- eliminé `TextFont` y saqué del núcleo la ruta propia de render de texto;
- simplifiqué `ResourceManager` al volver a una API más directa y menos estructurada para VAO, shaders y texturas;
- revertí `Shader` y `GameTexture2D` hacia un manejo más manual de compilación, uniforms y carga de imágenes;
- recorté el contrato base de `GameObject` y `GameScene`, retirando hooks de serialización y reinicio;
- eliminé varias utilidades accesorias del framework como offsets de cámara, reposicionamiento musical, drop callback y aliases globales ligados a subsistemas retirados.

Con esta etapa dejé el framework en una versión más contenida y defensiva: menos alcance funcional en el core, menos abstracciones experimentales y una API más simple sobre la cual seguir iterando después.