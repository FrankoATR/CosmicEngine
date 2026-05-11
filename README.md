# WandEngine

## Fecha efectiva

**31 de marzo de 2025**

## Descripción general

En esta etapa el cambio dominante ya no está en los managers de escena o persistencia, sino en la infraestructura interna del render. El framework refactoriza su capa gráfica inmediata para volver más uniforme la creación y consumo de shaders, VAO y texturas, desplazando parte de la lógica matemática y del estado de render hacia abstracciones más consistentes dentro del propio engine.

La evidencia visible de esta versión aparece en cuatro frentes concretos: `ResourceManager` deja de depender de shaders y geometría hardcodeados como miembros sueltos y pasa a inicializar recursos por clave; `Shader` evoluciona desde una envoltura mínima de programa OpenGL hacia una API con helpers de modelo y proyección; `GameTexture2D` asume directamente la carga y recarga de imagen desde ruta; y además el framework empieza a integrar render de texto dentro de la misma capa de recursos mediante `TextFont`, `RenderText()` y shaders maestros embebidos.

Con ello el proyecto no cambia de backend ni de stack externo, pero sí da un paso claro hacia un framework gráfico más ordenado internamente: los recursos dejan de estar repartidos en lógica ad hoc y empiezan a quedar encapsulados detrás de contratos reutilizables.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- normalizar la API interna de recursos gráficos para que el render 2D dependa menos de inicialización manual y más de registros reutilizables;
- mover transformaciones y proyección a una interfaz de shader más expresiva y menos repetitiva;
- integrar una primera ruta funcional de render de texto dentro del mismo subsistema de recursos, sin abrir todavía un manager separado.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `include/WandEngine/Managers/Resource/ResourceManager.*`, donde la inicialización de recursos por defecto se reorganiza alrededor de `Load_Static_VAO`, `LoadShaderFromCode`, `LoadShaderFromPath`, `LoadTexture`, `LoadTextureSheet`, `LoadTextFont` y `RenderText()`.
- `include/WandEngine/Managers/Resource/MasterShaders.hpp`, donde se centralizan shaders maestros embebidos para sprites, spritesheets, primitivas y texto.
- `include/WandEngine/Models/Shader/Shader.*`, donde el wrapper de programa OpenGL incorpora constructor con fuentes, `Use`, `EndUse`, `GetID`, `SetModel` y `SetProjection`.
- `include/WandEngine/Models/Texture/GameTexture2D.*`, donde la textura pasa a construirse directamente desde una ruta de imagen y añade `SetNewImage()` para recarga explícita del recurso.
- `include/WandEngine/Models/TextFont/*`, donde se encapsula la carga de glifos ASCII, las texturas por carácter y los buffers dinámicos necesarios para dibujar texto con FreeType.
- `include/WandEngine/Managers/DataBase/DataBaseManager.*`, donde se añade `ConsultTable()` como helper de consulta simple sobre tablas persistidas.
- `include/WandEngine/Models/Mesh/Mesh.cpp`, donde la capa de mallas se adapta al nuevo encapsulamiento de `Shader` usando `GetID()` en lugar de acceder al programa como estado público.

## Arquitectura o sistemas principales

### ResourceManager como registro real de recursos gráficos

La refactorización más visible de esta etapa cae en `ResourceManager`. El manager deja de depender de miembros concretos como un `spriteVAO` fijo y shaders residentes como estado directo de la clase, y pasa a organizar sus recursos a través de mapas indexados por clave.

Ese cambio se vuelve importante porque concentra en un solo punto operaciones que antes quedaban más dispersas o acopladas a una inicialización concreta:

- creación de VAO reutilizables por nombre;
- carga de shaders desde código o desde archivo;
- carga de texturas y spritesheets con una ruta uniforme;
- recuperación tipada de texturas, shaders, hojas de sprites y VAO;
- bootstrap explícito de recursos por defecto mediante `Init()` y un paquete de shaders maestros embebidos.

Con ello el framework gana una frontera más clara entre el subsistema que registra recursos y el subsistema que los consume durante el render.

### Shader como abstracción de transformaciones y proyección

`Shader` deja de ser solo un contenedor del programa OpenGL y pasa a asumir parte del trabajo repetitivo que antes quedaba incrustado en los call-sites del render. La nueva interfaz ya no se limita a compilar y subir uniforms sueltos: también encapsula helpers de más alto nivel como `SetModel()` y `SetProjection()`.

Eso tiene dos efectos prácticos dentro del engine:

- reduce duplicación en `ResourceManager` al construir matrices de modelo para sprites, hojas de sprites y primitivas;
- centraliza la relación entre el shader y la proyección activa del motor, en lugar de dejar esa responsabilidad repartida entre varios métodos de render.

Además, la clase deja de exponer su programa como dato utilizado informalmente y formaliza el acceso mediante `GetID()`, lo que vuelve más consistente la interacción con otras capas como `Mesh`.

### Texturas con ciclo de vida más directo

`GameTexture2D` también cambia de responsabilidad. En lugar de depender de una secuencia externa donde otra capa carga bytes, configura formato y luego llama a `Generate(width, height, data)`, la textura ahora puede nacer directamente desde una ruta de imagen.

La mejora técnica visible es doble:

- el recurso encapsula por sí mismo la carga inicial del archivo;
- el framework gana `SetNewImage()` como vía explícita para recargar el contenido sin reinstanciar toda la abstracción.

Eso simplifica tanto `LoadTexture()` como `LoadTextureSheet()` en `ResourceManager` y alinea mejor el ciclo de vida del objeto con su responsabilidad real.

### Render 2D menos repetitivo y mejor encapsulado

Buena parte del código nuevo en `ResourceManager` no añade efectos visuales nuevos, pero sí reduce la cantidad de lógica repetida por operación de dibujo. La composición de matrices, la selección de shader, la proyección y el binding de recursos pasan a reutilizar una interfaz más homogénea.

En la práctica, el render de sprites, spritesheets y primitivas queda más cerca de un contrato interno estable:

- el shader se obtiene por clave y se usa a través de `Use()` y `EndUse()`;
- el modelo se describe con posición, tamaño y rotación en una sola operación;
- la proyección ortográfica se solicita mediante una semántica explícita del engine;
- el acceso a VAO y texturas se hace por registro, no por miembros especiales dispersos.

### Texto integrado dentro del pipeline de recursos

El cambio de texto en esta etapa ya no se expresa como un manager aislado, sino como una capacidad absorbida por `ResourceManager`. El framework incorpora shaders maestros `text_vs` y `text_fs`, añade `LoadTextFont()` y `RenderText()`, y delega la carga de glifos y buffers a `TextFont`.

Eso deja una arquitectura más coherente con el resto del refactor:

- el texto usa el mismo registro de shaders y recursos que el resto del render;
- `TextFont` encapsula glifos ASCII, texturas por carácter y VAO/VBO dinámicos;
- la ruta de dibujo de texto queda alineada con la misma semántica de proyección ortográfica del engine.

El detalle importante es que el framework abandona la idea de un subsistema provisional `TextManager/Text` y pasa a integrar texto directamente dentro de la infraestructura gráfica que ya estaba consolidando.

### Persistencia con consulta más directa

`DataBaseManager` recibe una ampliación pequeña pero coherente con el resto del enfoque: `ConsultTable()` agrega una ruta más directa para consultar columnas de una tabla sin reconstruir manualmente la sentencia cada vez.

No es un cambio grande de subsistema, pero sí refuerza la idea de esta etapa: pequeñas responsabilidades repetidas se convierten en helpers propios del framework para reducir fricción en capas superiores.

### Compatibilidad interna con mallas y materiales

La capa `Mesh` apenas cambia una línea, pero esa línea es sintomática del refactor. El acceso al identificador del shader deja de hacerse sobre un campo público y pasa a utilizar `GetID()`. Es un ajuste pequeño, aunque coherente con la misma dirección general de la fecha: encapsular mejor el estado gráfico y reducir el acoplamiento estructural entre capas.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- el subsistema de recursos ya puede registrar y resolver shaders, texturas, spritesheets, fuentes de texto y VAO bajo una API más estable y homogénea;
- la capa de render ya puede delegar más lógica común a `Shader`, `GameTexture2D` y `TextFont`, reduciendo duplicación en las rutas de dibujo del engine.

Con ello dejé una base gráfica más limpia internamente. La mejora principal de esta etapa no está en una capacidad nueva muy visible hacia fuera, sino en el orden interno del framework: el render empieza a descansar sobre contratos más reutilizables y menos acoplados a inicializaciones manuales.

## Dependencias externas visibles

No aparecen dependencias nuevas visibles en esta etapa. El cambio está en cómo se usan mejor las ya presentes:

- `stb_image` queda más embebida en el ciclo de vida de `GameTexture2D`;
- OpenGL queda algo más encapsulado detrás de `Shader`, `GameTexture2D` y `ResourceManager`;
- `sqlite3` gana un helper adicional de consulta a través de `DataBaseManager`;
- FreeType queda incorporada de forma más visible a través de `TextFont` y la nueva ruta de render de texto.

## Resumen técnico de la versión

La etapa efectiva al **31 de marzo de 2025** queda delimitada por estos movimientos:

- reorganicé `ResourceManager` para registrar shaders, texturas, spritesheets y VAO mediante una API más uniforme;
- incorporé `MasterShaders.hpp` como paquete de shaders embebidos y añadí una ruta de render de texto dentro de `ResourceManager`;
- trasladé parte de la lógica de transformación y proyección a `Shader` con `SetModel()` y `SetProjection()`;
- volví `GameTexture2D` responsable de su propia carga y recarga de imágenes desde archivo;
- integré `TextFont` como base de glifos, buffers y texturas para texto, en lugar de mantener el experimento separado de `TextManager` y `Text`;
- añadí `ConsultTable()` a `DataBaseManager` como helper de consulta simple;
- ajusté `Mesh` para alinearse con el nuevo encapsulamiento del shader mediante `GetID()`.

Con esta etapa dejé el framework en una versión más coherente internamente para seguir creciendo sobre OpenGL: menos lógica de render dispersa, más encapsulamiento de recursos y una primera integración real de texto dentro del propio pipeline del motor.