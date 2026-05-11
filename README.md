# WandAllegroEngine

## Fecha efectiva

**4 de noviembre de 2024**

## Descripción general

En esta etapa reorganicé el sistema de eventos del framework para dejar de depender de observadores definidos por herencia y pasar a un registro de callbacks asociados por nombre. Con ese cambio el motor ya no necesita una interfaz dedicada para cada consumidor de eventos, sino que puede resolver disparo y suscripción desde el propio manager.

En paralelo incorporé una definición propia de color para el framework y la integré en `SceneManager`, con el fin de desacoplar la representación interna del color de la firma directa de Allegro dentro del manager de escenas.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- sustituir el modelo de eventos basado en objetos observadores por callbacks registrados por nombre;
- introducir tipos base propios del framework para datos compartidos de infraestructura;
- simplificar la integración del sistema de eventos en escenas y entidades sin exigir clases adaptadoras adicionales.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `src/WandAllegroEngine/Managers/`, donde `EventManager` abandona el almacenamiento de observadores concretos y pasa a resolver eventos mediante callbacks indexados por nombre, y donde `SceneManager` adopta un color propio del framework.
- `src/WandAllegroEngine/Interfaces/`, donde aparece `Definitions.hpp` con la estructura `WAND_COLOR` y donde la antigua interfaz `GameEvent` queda desplazada del flujo activo.
- `src/Events/`, donde `Logger` deja de formar parte efectiva del mecanismo vigente de eventos.

## Arquitectura o sistemas principales

### Sistema de eventos por callbacks

En `EventManager` reemplacé el esquema anterior por una tabla de callbacks indexada por nombre de evento. La interfaz activa del manager queda compuesta ahora por:

- `RegisterEvent(const std::string &eventName, EventCallback callback)`;
- `TriggerEvent(const std::string &eventName)`;
- `RemoveEvent(const std::string &eventName)`;
- `Clear()`.

Con este ajuste dejé de almacenar instancias de observadores concretos y pasé a resolver la suscripción desde funciones registradas dinámicamente. El efecto inmediato es un sistema de eventos más liviano y menos acoplado a jerarquías de clases.

### Retiro del contrato basado en herencia

La interfaz `GameEvent` y la implementación `Logger` dejan de formar parte efectiva del flujo activo del sistema. Ambas quedaron fuera de uso directo dentro del árbol visible, lo que confirma el desplazamiento del modelo anterior basado en objetos con `OnNotify()`.

Con ello eliminé la necesidad de definir una clase derivada por cada consumidor del sistema de eventos cuando basta con registrar una acción puntual.

### Definición propia de color

En `Definitions.hpp` incorporé `WAND_COLOR` como estructura base del framework para representar color mediante componentes `r`, `g`, `b` y `a`.

Sobre esa base, `SceneManager` deja de conservar `ALLEGRO_COLOR` como estado interno y pasa a trabajar con `WAND_COLOR`, convirtiéndolo a color de Allegro únicamente en el punto de dibujo real mediante `al_map_rgba(...)`.

Con este ajuste reduje la dependencia directa de tipos externos dentro de la API interna del manager de escenas.

### Ajuste mínimo de infraestructura

Además de lo anterior, `Size` incorpora un constructor por defecto y `GameManager` deja de inicializar manualmente ese tamaño desde la lista de inicialización. El cambio no altera el rol del manager, pero sí deja su construcción más alineada con el resto de tipos básicos del framework.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en dos sentidos:

- las escenas y entidades pueden registrar acciones asociadas a eventos concretos sin crear observadores dedicados;
- el color de fondo de escena ya puede configurarse mediante una estructura propia del framework en lugar de depender de `ALLEGRO_COLOR` como tipo de intercambio interno.

Con ello dejé una base más directa para integrar reacciones de gameplay y estados visuales desde la propia capa del framework, con menos clases accesorias y menor acoplamiento a tipos externos.

## Dependencias externas visibles

No incorporé dependencias externas visibles nuevas en esta etapa.

## Resumen técnico de la versión

La etapa efectiva al **4 de noviembre de 2024** queda delimitada por estos movimientos:

- reemplacé el sistema de eventos basado en observadores por un registro de callbacks asociados por nombre;
- retiré del flujo activo la dependencia efectiva de `GameEvent` y de implementaciones concretas como `Logger`;
- incorporé `WAND_COLOR` como tipo base del framework para representar color;
- adapté `SceneManager` para conservar y consumir color propio del framework en lugar de `ALLEGRO_COLOR` como estado interno;
- añadí un constructor por defecto a `Size` para simplificar la inicialización de infraestructura básica.

Con esta etapa dejé el framework más coherente en su sistema de eventos y un poco menos dependiente de tipos externos en su API interna, especialmente en los puntos donde la infraestructura del motor necesita exponer mecanismos reutilizables a las escenas.