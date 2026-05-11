# WandEngine

## Fecha efectiva

**6 de abril de 2025**

## Descripción general

En esta etapa el cambio dominante deja de estar en la infraestructura de render y pasa a la persistencia estructurada de objetos. El framework empieza a formalizar una ruta de serialización por tipo, donde la base de datos ya no solo ejecuta SQL auxiliar, sino que puede registrar metadatos de clases, guardar instancias por clase y reconstruir objetos del runtime a partir de constructores declarados.

La evidencia visible de esta versión se concentra sobre todo en `DataBaseManager` y `GameObject`. `DataBaseManager` incorpora un registro de serialización con columnas declaradas y constructores por clase; `GameObject` añade `GetAllValues()` como hook virtual para exportar el estado persistible de cada instancia; y además `Draw()` pasa a ser `const`, reforzando la separación entre render y mutación del objeto.

Con ello el proyecto no cambia de backend ni de stack, pero sí sube de nivel la capa de persistencia del engine: deja de depender solo de consultas manuales y empieza a perfilar una semántica propia de serialización orientada a tipos del framework.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- formalizar un registro de serialización por clase dentro del motor;
- hacer que el modelo base de objetos exponga una salida uniforme de valores persistibles;
- preparar la reconstrucción de instancias desde base de datos sin acoplar esa lógica a una implementación concreta de escena.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `include/WandEngine/Managers/DataBase/DataBaseManager.*`, donde el manager añade `serialization_resources`, `RegisterSerialization()`, nuevas variantes de `SaveObjectsData()` y `LoadObjectsData()`, y una `ConsultTable()` capaz de transportar contexto vía `void* data`.
- `include/WandEngine/Models/GameObject.*`, donde el modelo base incorpora `GetAllValues()` como contrato virtual de exportación de valores y hace `Draw()` const-correct.
- `include/WandEngine/Managers/Resource/ResourceManager.hpp`, donde aparece un helper inline `RS_MNX()` para acceso corto al singleton de recursos, aunque sin alterar la arquitectura principal del framework.

## Arquitectura o sistemas principales

### Registro declarativo de serialización

El cambio técnico más fuerte de esta etapa está en `DataBaseManager`. La persistencia deja de limitarse a abrir una base de datos, crear tablas o ejecutar consultas puntuales, y empieza a adoptar una estructura declarativa por clase. Con `serialization_resources`, el manager ya puede mantener para cada tipo registrado:

- la lista de columnas y sus tipos SQL;
- un constructor capaz de reconstruir una instancia a partir de `char** argv`;
- una relación explícita entre el nombre de clase del runtime y su representación persistida.

Eso mueve la persistencia desde un uso manual de SQLite hacia una capa del framework con semántica propia de serialización.

### Guardado y carga por clase del runtime

`SaveObjectsData()` y `LoadObjectsData()` ya no son placeholders genéricos. En esta versión pasan a trabajar por nombre de clase y se apoyan en el registro de serialización para decidir cómo crear tablas, qué columnas persistir y cómo reconstruir instancias.

La ruta de guardado ahora expresa un flujo mucho más definido:

- localizar objetos por `ClassName` dentro de `ObjectManager`;
- derivar columnas desde el metamodelo registrado;
- crear la tabla correspondiente si no existe;
- obtener valores serializables desde cada objeto;
- insertar cada fila en la tabla adecuada.

La ruta de carga sigue la misma dirección en sentido inverso: consulta las columnas registradas y usa un constructor por clase para reinstanciar objetos en el manager. Eso ya no es solo utilería de base de datos; es infraestructura de runtime.

### GameObject como contrato base de persistencia

El ajuste en `GameObject` es pequeño en líneas, pero importante en diseño. `GetAllValues()` aparece como método virtual y devuelve la interfaz mínima que la persistencia necesita del modelo base: una lista ordenada de valores serializables para una instancia.

La ventaja de esta decisión es que el engine ya no necesita que cada ruta de guardado conozca por fuera cómo extraer el estado de un objeto concreto. El contrato queda empujado al propio modelo base y listo para ser especializado por clases derivadas.

Eso convierte a `GameObject` en una pieza central del nuevo flujo de serialización, aunque la implementación por defecto todavía sea vacía.

### Separación más estricta entre render y mutación

En la misma línea de endurecer contratos base, `Draw()` pasa a ser `const`. El cambio no altera una API enorme, pero sí comunica mejor la intención del framework: dibujar un objeto no debería modificar su estado observable.

Ese tipo de const-correctness importa más en esta etapa porque el motor empieza a combinar runtime, persistencia y reconstrucción. Cuanto más clara sea la separación entre consultar, dibujar, serializar y mutar, más defendible será la evolución de la arquitectura.

### Consultas con contexto y utilería de bajo ruido

`ConsultTable()` también se refina al aceptar un puntero de contexto, lo que permite callbacks de carga con información adicional sin depender de variables globales o cierres externos imposibles para la firma de SQLite. Eso es justo lo que habilita que `LoadObjectsData()` use un callback genérico y aun así sepa qué clase está reconstruyendo.

El helper inline `RS_MNX()` en `ResourceManager.hpp` es mucho menor en importancia, pero confirma la misma tendencia general del código de esta fecha: se reducen pequeñas fricciones de uso del framework mientras los contratos centrales se vuelven más explícitos.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- la persistencia del engine ya puede registrar cómo se serializa y reconstruye cada clase del runtime;
- el modelo base de objetos ya ofrece un punto de extensión directo para que esa persistencia no dependa de extracción manual de datos por fuera del objeto.

Con ello dejé una base de persistencia mucho más madura que la de febrero. La mejora principal de esta etapa no está en el render, sino en que el framework empieza a tratar serialización y reconstrucción de objetos como una responsabilidad interna del motor y no solo como SQL suelto alrededor del juego.

## Dependencias externas visibles

No aparecen dependencias nuevas visibles en esta etapa. El cambio está en cómo se usan con más rigor las ya presentes:

- `sqlite3` deja de actuar solo como backend de consultas y pasa a sostener un registro de serialización más estructurado dentro de `DataBaseManager`.

## Resumen técnico de la versión

La etapa efectiva al **6 de abril de 2025** queda delimitada por estos movimientos:

- incorporé un registro de serialización por clase en `DataBaseManager` con columnas declaradas y constructores de reconstrucción;
- convertí `SaveObjectsData()` y `LoadObjectsData()` en rutas operativas por tipo del runtime, en lugar de placeholders genéricos;
- añadí `GetAllValues()` a `GameObject` como contrato base de exportación de estado persistible;
- hice `Draw()` const-correct para separar mejor render y mutación del objeto;
- amplié `ConsultTable()` para transportar contexto a callbacks de carga;
- mantuve un ajuste menor de ergonomía en `ResourceManager` con el helper inline `RS_MNX()`.

Con esta etapa dejé el framework en un punto importante de madurez interna: el motor ya no solo administra render y recursos, sino que empieza a definir de forma explícita cómo sus objetos se describen, se guardan y se reconstruyen desde persistencia.