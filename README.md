# WandEngine

## Fecha efectiva

**2 de abril de 2025**

## Descripción general

En esta etapa el cambio dominante no es la aparición de un subsistema nuevo, sino la consolidación del refactor gráfico anterior. El framework toma la infraestructura de recursos y texto introducida al cierre de marzo y la vuelve más estable desde el punto de vista interno: separa recursos gráficos estáticos de buffers dinámicos, normaliza claves internas del motor y mejora la limpieza del ciclo de vida de recursos GPU.

La evidencia visible de esta versión se concentra sobre todo en `ResourceManager` y `TextFont`. `ResourceManager` abandona definitivamente la lógica especial por forma y pasa a operar con registros explícitos de VAO estáticos y VAO/VBO dinámicos; además adopta claves reservadas del motor con prefijo `WAND_` para shaders y geometría interna. En paralelo, `TextFont` deja de ser solo un cargador de glifos funcional y pasa a responsabilizarse también por la liberación correcta de texturas de caracteres.

Con ello el proyecto no amplía demasiado la superficie pública del framework, pero sí endurece la parte que ya estaba creciendo: el render se vuelve más consistente, menos ambiguo y menos propenso a fugas o acoplamientos implícitos.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- distinguir con claridad recursos de geometría estática y buffers dinámicos dentro del pipeline 2D del engine;
- formalizar nombres reservados y rutas internas del motor para evitar colisiones con recursos del usuario;
- mejorar la administración de memoria y destrucción de recursos gráficos asociados a texto y render inmediato.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `include/WandEngine/Managers/Resource/ResourceManager.*`, donde el manager separa `static_vao_resources` de `dynamic_vao_vbo_resources`, introduce `Load_Dynamic_VAO_VBO()` con conteo de vértices y centraliza limpieza mediante `ClearMap()`.
- `include/WandEngine/Managers/Resource/ResourceManager.*`, donde las primitivas internas y shaders por defecto pasan a registrarse bajo claves reservadas como `WAND_Sprite`, `WAND_Shape`, `WAND_Text`, `WAND_Point`, `WAND_Line`, `WAND_Triangle` y `WAND_Rectangle`.
- `include/WandEngine/Models/TextFont/TextFont.cpp`, donde el modelo de fuente ya libera las texturas de glifos al destruirse y deja más cerrada la responsabilidad del recurso.

## Arquitectura o sistemas principales

### Separación entre geometría estática y dinámica

El cambio técnico más claro de esta etapa está en la forma en que `ResourceManager` entiende la geometría interna del engine. La versión anterior ya registraba recursos por clave, pero todavía no distinguía con suficiente precisión entre:

- VAO estáticos reutilizables para sprites y geometría fija;
- VAO/VBO dinámicos que necesitan recibir datos nuevos en cada operación de render inmediato.

Con la introducción de `static_vao_resources` y `dynamic_vao_vbo_resources`, esa diferencia pasa a ser explícita. Esto mejora la semántica del framework porque deja de tratar líneas, rectángulos, triángulos y puntos como casos especiales dispersos y los convierte en recursos dinámicos declarados desde `Init()`.

### Render inmediato más ordenado para primitivas

La consecuencia directa de esa separación es que `RenderShape()` deja de depender de estructuras auxiliares dedicadas por forma y pasa a trabajar contra un registro dinámico por clave. El flujo ahora es más coherente:

- se resuelve el VAO/VBO dinámico por nombre reservado del motor;
- se actualiza el contenido del buffer con `glBufferSubData`;
- se reutiliza el mismo camino de shader y transformación para dibujar puntos, líneas, triángulos y rectángulos.

Eso reduce estado implícito y hace más predecible el comportamiento del render inmediato dentro del engine.

### Namespacing interno de recursos del motor

Otro ajuste importante, aunque más discreto, es la adopción de claves internas con prefijo `WAND_`. Los shaders y recursos creados por el framework ya no viven con nombres genéricos como `Sprite`, `Shape` o `text`, sino bajo identificadores explícitamente reservados para el motor.

Esto aporta dos ventajas estructurales:

- evita colisiones conceptuales con recursos que una capa superior pudiera registrar por su cuenta;
- vuelve más clara la frontera entre recursos del engine y recursos del proyecto que consume el engine.

No es un cambio visual, pero sí una mejora real de contrato interno.

### Texto integrado con limpieza de recursos más completa

La ruta de texto introducida al final de marzo se consolida en abril. En esta etapa no aparece una API nueva radical, pero sí se fortalece su implementación: `TextFont` ahora destruye explícitamente las texturas generadas para cada glifo y limpia su mapa interno al destruirse.

Eso importa porque el texto en este diseño depende de:

- texturas OpenGL por carácter ASCII;
- buffers dinámicos para los quads de glifos;
- una vida útil asociada al registro y eliminación de fuentes dentro de `ResourceManager`.

Al cerrarse mejor ese ciclo, el subsistema de texto deja de ser solo funcional y pasa a ser más seguro desde el punto de vista del runtime.

### Limpieza genérica de mapas de recursos

`ResourceManager` también introduce una utilidad de limpieza genérica mediante `ClearMap()`. El cambio parece pequeño, pero expresa una intención clara del framework: dejar de repetir bucles manuales para cada mapa de recursos y mover esa responsabilidad a una operación más uniforme.

Con ello el código de destrucción gana consistencia en varias familias de recursos:

- shaders;
- texturas;
- hojas de sprites;
- fuentes de texto;
- VAO y VBO internos.

Ese tipo de refactor no agrega capacidad visible al usuario final, pero sí reduce el riesgo de errores de mantenimiento en la infraestructura del motor.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- el render 2D del motor ahora separa mejor qué recursos son persistentes y cuáles se actualizan cuadro a cuadro;
- la capa de recursos ya administra con más disciplina tanto la nomenclatura interna del engine como la destrucción de buffers, texturas y glifos.

Con ello dejé una base gráfica más robusta que la del 31 de marzo. La mejora principal de esta etapa no está en añadir otra capacidad grande, sino en hacer más confiable la que ya se había abierto en recursos, primitivas y texto.

## Dependencias externas visibles

No aparecen dependencias nuevas visibles en esta etapa. El cambio está en cómo se usan con más rigor las ya presentes:

- OpenGL queda mejor encapsulado en la gestión de VAO estáticos y VBO dinámicos del `ResourceManager`;
- FreeType mantiene su papel dentro de `TextFont`, pero ahora con un cierre más completo del ciclo de vida de glifos y texturas.

## Resumen técnico de la versión

La etapa efectiva al **2 de abril de 2025** queda delimitada por estos movimientos:

- separé los recursos geométricos internos del engine entre VAO estáticos y VAO/VBO dinámicos;
- reemplacé la lógica auxiliar por forma en `ResourceManager` por una ruta unificada de render inmediato apoyada en buffers dinámicos;
- normalicé nombres reservados del motor con prefijo `WAND_` para shaders y geometría interna;
- reforcé la limpieza del subsistema de recursos con `ClearMap()` y destrucción más homogénea de mapas internos;
- completé la liberación de texturas de glifos en `TextFont`, consolidando mejor la ruta de render de texto.

Con esta etapa dejé el framework en una versión más disciplinada internamente: misma dirección gráfica que la del 31 de marzo, pero con una separación más clara entre recursos, menos estado implícito y mejor administración del ciclo de vida en GPU.