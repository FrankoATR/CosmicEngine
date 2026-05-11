# WandEngine

## Fecha efectiva

**28 de abril de 2025**

## Descripción general

En esta etapa el cambio dominante deja de estar en la normalización nominal del core y pasa a la consolidación de un pipeline gráfico y espacial claramente orientado a 3D. El framework centraliza cámara e iluminación en managers más fuertes, amplía `ResourceManager` para que el render de modelos y geometría tridimensional sea parte de la infraestructura estándar, y reorganiza shaders y utilidades de soporte alrededor de esa nueva ambición.

La evidencia visible de esta versión se concentra sobre todo en tres decisiones: desaparecen modelos específicos de cámara y luz en favor de `CameraManager` y `LightManager` como puntos centrales de control; `ResourceManager` incorpora carga y render de modelos 3D, primitivas volumétricas y una variante explícita de render 2D para UI; y `MasterShaders.hpp` pasa a reunir un set de shaders más amplio, incluyendo rutas diferenciadas para 2D, 3D, texto y modelos iluminados.

Con ello el proyecto no solo conserva la configuración 2D/3D introducida antes, sino que la convierte en una capacidad práctica del pipeline. El framework queda más cercano a un motor con soporte tridimensional integrado y no solo configurable a nivel de tipos base.

## Objetivo técnico

Esta etapa queda centrada en tres frentes:

- mover cámara e iluminación desde modelos especializados a managers de alcance global;
- integrar el render 3D y la carga de modelos dentro del flujo estándar de recursos del engine;
- alinear shaders, entrada y colisiones auxiliares con una arquitectura más centrada en managers y en espacio tridimensional.

## Estructura del proyecto

La diferencia visible respecto a la etapa anterior se concentra en estos puntos:

- `include/WandEngine/Managers/Camera/CameraManager.hpp`, donde la cámara absorbe de forma centralizada el comportamiento 2D y 3D que antes estaba repartido en modelos específicos.
- `include/WandEngine/Managers/Light/LightManager.hpp` y `include/WandEngine/Models/Light/Light.hpp`, donde la iluminación pasa a gestionarse como un subsistema del engine con registro de shaders y parámetros globales de ambiente.
- `include/WandEngine/Managers/Resource/ResourceManager.hpp`, donde aparecen `Load3DModel()`, `Render3DModel()`, render de paralelepípedos y una ruta explícita de sprite para UI.
- `include/WandEngine/Managers/Resource/MasterShaders.hpp`, donde se amplía el repertorio de shaders embebidos para cubrir formas 3D, texto, sprites y modelos con iluminación.
- `include/WandEngine/Models/Model/Model.hpp`, que deja de ser solo soporte lateral del árbol gráfico y pasa a integrarse de forma más directa en el pipeline estándar de recursos.
- `include/WandEngine/Managers/Input/` y `include/WandEngine/Collisions/GameGridCollisions.*`, que ajustan comportamiento de soporte para convivir con la nueva organización del runtime, aunque sin convertirse en el eje dominante del cambio.

## Arquitectura o sistemas principales

### Cámara centralizada en CameraManager

El cambio técnico más visible de esta etapa está en la absorción del comportamiento de cámara dentro de `CameraManager`. Las variantes especializadas previas dejan de ser el punto principal de modelado, y el manager pasa a contener directamente matrices de vista y proyección, estado de navegación y rutinas de entrada para mover la cámara según el perfil activo.

Eso endurece la arquitectura del framework en un sentido claro: la cámara deja de ser tratada como un actor o modelo separado y pasa a ser infraestructura global del motor. En una base cada vez más orientada a 3D, esa centralización reduce dispersión y alinea mejor el control del espacio de render.

### Iluminación como subsistema del engine

La misma lógica se aplica a la luz. `LightManager` gana peso como subsistema propio y ya no se limita a contener referencias sueltas. En esta versión incorpora parámetros globales de iluminación ambiente, dirección, difuso y especular, además de un registro explícito de shaders objetivo.

La consecuencia importante es que la luz ya no se presenta como una entidad decorativa o aislada, sino como una capacidad transversal del pipeline gráfico. El framework empieza a modelar iluminación como política central del render, no como un detalle que cada shader o escena resuelve por fuera.

### ResourceManager como punto único para recursos 2D y 3D

`ResourceManager` vuelve a crecer, pero ahora con un enfoque más nítido que en etapas anteriores. Ya no se expande solo para texto, sprites o UI, sino para incorporar recursos y draw calls tridimensionales en la misma superficie pública del manager.

La presencia de `Load3DModel()` y `Render3DModel()` lo deja claro: la carga de modelos importados entra al flujo estándar del engine. A eso se suman primitivas volumétricas como el render de paralelepípedos, que ayudan a cerrar la brecha entre utilidades 2D previas y un pipeline con geometría 3D explícita.

Esto hace que el manager de recursos deje de estar sesgado hacia un motor 2D expandido y se acerque más a una infraestructura híbrida de propósito general.

### MasterShaders como repertorio coordinado del pipeline

`MasterShaders.hpp` también cambia de papel. En lugar de ser solo una colección auxiliar, pasa a expresar de forma más concreta las variantes shader que el motor necesita para operar en 2D y 3D bajo una misma arquitectura.

El repertorio visible incluye:

- shaders para sprites y spritesheets;
- shaders para formas 2D y 3D;
- shaders de texto;
- shaders para modelos 3D iluminados.

Eso importa porque muestra que la expansión del 28 de abril no es solo API pública. También hay una consolidación interna del pipeline para que los distintos tipos de render compartan una base coherente.

### Model como recurso de primer nivel

`Model` ya estaba presente en el árbol del framework, pero en esta fecha su papel cambia de categoría. Al integrarse con `ResourceManager`, deja de operar solo como una pieza disponible para usos puntuales y pasa a convertirse en un recurso de primer nivel dentro del engine.

Esa diferencia es importante: no estoy documentando la mera existencia de importación 3D, sino el momento en que el framework la incorpora a su flujo estándar de carga, almacenamiento y render.

### Input y colisiones como soporte de la nueva arquitectura

`InputManager` y `GameGridCollisions` también se ajustan en esta etapa, pero actúan como temas secundarios. Su función principal aquí es acompañar la centralización de cámara, la nueva semántica de objetos y la expansión del render, no abrir una línea arquitectónica independiente.

Conviene dejarlo así para no sobredimensionar cambios auxiliares que, aunque reales, están al servicio de la reorganización principal del pipeline.

## Integración del framework

Las implementaciones visibles del framework ya aprovechan estas capacidades en tres sentidos:

- la cámara y la iluminación pasan a integrarse como servicios globales del engine y no como modelos dispersos;
- el pipeline estándar de recursos ya puede cargar y dibujar modelos 3D junto con sprites, texto y primitivas;
- el stack de shaders del motor queda mejor alineado con una ejecución híbrida entre UI, 2D y escena 3D.

Con ello dejé una base de engine más madura en su capa gráfica que la del 21 de abril. La mejora principal de esta etapa no está en la nomenclatura ni en la configuración, sino en volver operativa una arquitectura 3D centralizada dentro del propio framework.

## Dependencias externas visibles

No aparece un cambio completo de stack, pero sí una profundización clara del uso de varias dependencias ya presentes:

- OpenGL sigue siendo la base del render, ahora con un pipeline más amplio para escena 3D, UI y modelos iluminados;
- Assimp gana peso práctico al integrarse los modelos 3D al flujo estándar de `ResourceManager`;
- GLFW e input siguen sosteniendo navegación y eventos, aunque aquí actúan principalmente como soporte del sistema de cámara;
- el resto de subsistemas previos del core permanecen activos, pero no son el centro del cambio de esta fecha.

## Resumen técnico de la versión

La etapa efectiva al **28 de abril de 2025** queda delimitada por estos movimientos:

- centralicé la lógica de cámara en `CameraManager` y retiré el peso de modelos específicos para esa responsabilidad;
- reforcé `LightManager` como subsistema global de iluminación con parámetros ambiente y registro de shaders;
- amplié `ResourceManager` para cargar y renderizar modelos 3D, además de primitivas volumétricas y una ruta explícita de sprite UI;
- consolidé `MasterShaders.hpp` como base interna del pipeline para 2D, 3D, texto y modelos iluminados;
- elevé `Model` a recurso de primer nivel dentro del flujo estándar del engine;
- mantuve ajustes secundarios en input y colisiones para acompañar la nueva arquitectura centrada en managers.

Con esta etapa dejé el framework en un punto claramente más sólido para trabajo tridimensional: cámara e iluminación gestionadas desde el núcleo, recursos 3D integrados al pipeline común y una infraestructura shader más coherente para sostener esa evolución.