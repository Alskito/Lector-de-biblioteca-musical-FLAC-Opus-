# Lector de biblioteca musical (FLAC/Opus)

El programa lee tu música en FLAC y Opus con TagLib, la ordena por título, artista, álbum, año o número de pista, busca canciones por título, artista o álbum, arma una cola de reproducción, reproduce el audio y guarda listas de reproducción en archivos `.m3u8`.

En este avance el programa:

1. Busca archivos FLAC y Opus en la carpeta que elijas (y en sus subcarpetas).
2. Lee sus datos con TagLib y los guarda en un `Song` dentro de un `std::vector`.
3. Ordena el vector con Merge Sort.
4. Busca canciones por título, artista o álbum.
5. Copia las canciones a una lista doblemente ligada (`DList`) que funciona como cola de reproducción.
6. Deja armar una lista de reproducción con las canciones que elijas, guardarla como `.m3u8`, volver a abrirla después y reproducirla con la misma cola.
7. Reproduce el audio con SDL2_mixer (decodifica FLAC y Opus). 

El programa es de terminal, con menús de texto.
Genuinamente intenté hacer una interfaz, pero fue un dolor. Le invertí unas cuantas horas y no me gustó; tal vez en el futuro lo intente otra vez, pero para esta revisión no lo haré.

### Dónde está cada cosa

| Archivo | Qué contiene |
|---|---|
| `main.cpp` | Menús, lista de reproducción (vector), comandos de la cola (`playQueue`), abrir y guardar playlists. |
| `metadata.cpp/.hpp` | `scanFolder`: lee la carpeta y los metadatos con TagLib. |
| `sorter.cpp/.hpp` | `mergeSort` y `comparatorFor` (los cinco criterios). |
| `search.cpp/.hpp` | `findSongs`: búsqueda por título, artista o álbum. |
| `dlist.h`, `exception.h` | Lista doblemente ligada y excepciones (material de clase). |
| `playbackQueue.hpp` | `PlaybackQueue`: la cola, con un cursor sobre `DList`. |
| `playlist.cpp/.hpp` | `savePlaylistM3u8` (escribir) y `loadPlaylistM3u8` (leer). |
| `audio.cpp/.hpp` | `AudioPlayer`: envoltorio de SDL2_mixer (único archivo que incluye SDL). |
| `song.hpp` | La estructura `Song`. |

## Cómo usar el programa

### Requisitos

Un compilador de C++ con soporte para C++17 y la librería TagLib **1.10 o superior** (se usa `lengthInSeconds()`). Para el audio, además, **SDL2** y **SDL2_mixer** (esta última con soporte de FLAC y Opus).

### Instalar dependencias

**Windows**

1. Instala MSYS2
2. Abre la terminal **MSYS2 MINGW64** (no la MSYS2 normal) y corre:

`pacman -Syu`

`pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-taglib mingw-w64-x86_64-pkg-config mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer`

**Linux — Arch:** `sudo pacman -S base-devel taglib sdl2 sdl2_mixer`

**Linux — Ubuntu/Debian:** `sudo apt install g++ libtag1-dev pkg-config libsdl2-dev libsdl2-mixer-dev`

**macOS** (con Homebrew): `brew install taglib pkg-config sdl2 sdl2_mixer`

### Compilar

Deja en la misma carpeta: `main.cpp`, `audio.cpp/.hpp`, `metadata.cpp/.hpp`, `sorter.cpp/.hpp`, `search.cpp/.hpp`, `playlist.cpp/.hpp`, `song.hpp`, `dlist.h`, `playbackQueue.hpp` y `exception.h`.

**Linux / macOS**

`g++ -std=c++17 main.cpp audio.cpp metadata.cpp sorter.cpp search.cpp playlist.cpp -o lector -pthread $(pkg-config --cflags --libs taglib SDL2_mixer)`

**Windows (terminal MSYS2 MINGW64)**

`g++ -std=c++17 main.cpp audio.cpp metadata.cpp sorter.cpp search.cpp playlist.cpp -o lector $(pkg-config --cflags --libs taglib) -I/mingw64/include/SDL2 -lSDL2_mixer -lSDL2`

**Windows (PowerShell, con MSYS2 ya agregado al PATH)**

`g++ -std=c++17 -IC:\msys64\mingw64\include -IC:\msys64\mingw64\include\SDL2 main.cpp audio.cpp metadata.cpp sorter.cpp search.cpp playlist.cpp -o lector.exe -LC:\msys64\mingw64\lib -ltag -lSDL2_mixer -lSDL2`

**Con CMake** (alternativa, requiere `pkg-config`; el `CMakeLists.txt` va en la misma carpeta):

`cmake -S . -B build`

`cmake --build build`

El ejecutable queda en `build/lector` (en Windows, `build/lector.exe`). En Windows con MSYS2 hay que tener instalados `cmake`, `ninja` y `pkgconf` y usar `cmake -S . -B build -G Ninja`.

### Ejecutar

`./lector "<ruta_a_tu_carpeta_de_musica>"`

Al iniciar, el programa te pregunta cómo ordenar (1 título, 2 artista, 3 álbum, 4 año, 5 pista). Después puedes cambiarlo con la opción 2 del menú.

Si la ruta no existe o no es una carpeta, el programa lo avisa y termina sin error. Después de ordenar se abre el menú principal:

1. **Mostrar la biblioteca** en el orden elegido.
2. **Cambiar el criterio de orden.**
3. **Buscar una canción** por título, artista o álbum (basta con escribir una parte del texto; no importan las mayúsculas).
4. **Reproducir la cola** reproduce la biblioteca en el orden elegido. Muestra el estado (`Stopped`, `Playing` o `Paused`), la canción actual, su duración y en qué lugar va.
5. **Lista de reproducción:** agregar canciones, mostrar, quitar, guardar como `.m3u8` abrir una lista guardada (se muestran los `.m3u8` de la carpeta de música y eliges uno) y reproducir la lista con la misma cola 
6. **Salir.**

**Comandos de la cola.** Cada uno acepta la letra o la palabra completa, con mayúsculas o minúsculas:

| Comando | Qué hace |
|---|---|
| `p`, `play` o `pause` | Play/pausa: si está detenida, empieza la canción actual desde el inicio; si suena, pausa; si está en pausa, continúa. |
| `s` o `stop` | Detiene la canción. Con `p` empieza de nuevo desde el inicio. |
| `n` o `next` | Siguiente. |
| `b`, `back`, `prev` o `previous` | Anterior. |
| `r` o `remove` | Quita la canción actual de la cola. |
| `a` o `add` | Agrega una canción justo **después** de la actual (`PlaybackQueue::addAfter`). Escribes un texto, se muestran las coincidencias de la biblioteca y eliges una por número. |
| `i` o `insert` | Igual, pero la agrega justo **antes** de la actual (`PlaybackQueue::addBefore`). |
| `e`, `exit`, `q` o `quit` | Sale de la cola y detiene la música. |

Agregar no cambia lo que está sonando, solo la cola. Siguiente, anterior y quitar siguen una regla: si algo estaba sonando, la nueva canción suena sola; si estaba detenida o en pausa, solo cambia la selección.

Cuando una canción termina, empieza la siguiente de la cola sola; al terminar la última, el programa avisa `End of the queue.` y se detiene. Si un archivo no se puede reproducir, el mensaje aparece arriba y el programa queda detenido en esa canción.

La lista se guarda dentro de la carpeta de música, porque las rutas dentro del archivo son relativas a esa carpeta. Así otros reproductores pueden abrirla.

### Limitaciones conocidas

- Si la ruta no existe o una subcarpeta no se puede leer, el programa muestra un mensaje y termina; no sigue con la biblioteca incompleta.
- El orden alfabético compara bytes en minúsculas con `std::tolower`, así que las letras con acento ("Ángel", "Émile") quedan después de la "z".
- Las canciones sin año o sin número de pista guardan 0 y salen al principio al ordenar por esos criterios.
- Se ordena por un solo criterio a la vez.
- La lista de reproducción solo vive en memoria hasta que la guardas.
- La búsqueda distingue las letras con acento, igual que el orden: buscar `angel` no encuentra "Ángel".
- No hay shuffle, repeat, volumen ni barra de progreso: solo play/pausa, detener, siguiente y anterior.
- Lo que agregas con `a` o `i` vive en la cola mientras la reproduces; para conservarlo hay que armar una lista de reproducción y guardarla.

### Créditos

`dlist.h` y `exception.h` vienen del material visto en clase (`exception.h` es de pperezm).

## SICT0302: Toma decisiones

### Selecciona un algoritmo de ordenamiento adecuado al problema

El programa debe ordenar una lista de canciones (mi biblioteca tiene aproximadamente 1100, pero el programa debería funcionar bien con bibliotecas mucho más grandes) por cinco criterios distintos: título, artista, álbum, año y pista. La decisión se basa en comparar la complejidad temporal de los algoritmos candidatos.

| Algoritmo | Mejor caso | Caso promedio | Peor caso | Espacio extra | Estable |
|---|---|---|---|---|---|
| Bubble Sort | O(n) | O(n²) | O(n²) | O(1) | Sí |
| Insertion Sort | O(n) | O(n²) | O(n²) | O(1) | Sí |
| Quick Sort | O(n log n) | O(n log n) | O(n²) | O(log n) | No |
| Heap Sort | O(n log n) | O(n log n) | O(n log n) | O(1) | No |
| **Merge Sort** | **O(n log n)** | **O(n log n)** | **O(n log n)** | **O(n)** | **Sí** |

**Descarto Bubble Sort e Insertion Sort** porque su costo promedio y de peor caso es O(n²), y en este programa la lista casi nunca llega ordenada: `scanFolder` devuelve las canciones en el orden de las carpetas y cada vez que el usuario cambia de criterio hay que ordenar desde cero.

**Descarto Quick Sort** porque su peor caso es O(n²).

**Descarto Heap Sort** porque no es estable y en la práctica accede a la memoria de forma menos ordenada.

**Elijo Merge Sort** por dos razones:

1. **Siempre es O(n log n).** No importa cómo venga ordenada la biblioteca.
2. **Es estable.** Si dos canciones empatan (por ejemplo, del mismo año), se quedan en el orden en que venían, que es el orden en que `scanFolder` las encontró en la carpeta. Eso hace el resultado predecible y permitiría encadenar ordenamientos (por ejemplo, ordenar primero por pista y luego por álbum deja las pistas de cada álbum en orden). Hoy el programa ordena por un solo criterio, así que la estabilidad por sí sola no garantiza que las pistas de un álbum salgan en orden cuando se ordena por álbum.

*Nota: Merge Sort está en la función `mergeSort()` del archivo `sorter.cpp`. Los cinco criterios usan el mismo algoritmo; solo cambia la función de comparación (`comparatorFor`).*

### Selecciona una estructura de datos adecuada al problema y la usa correctamente

Uso tres estructuras, porque la biblioteca, la cola y la lista de reproducción necesitan cosas distintas.

**Biblioteca: `std::vector<Song>`.** Se llena una vez, se ordena y se imprime. Para eso hay que agregar al final y entrar a cualquier posición en O(1), que es lo que necesita mi implementación de Merge Sort (trabaja con índices). Con una lista ligada cada acceso por posición costaría O(n) y ordenar subiría a O(n² log n). Merge Sort sí se puede hacer sobre una lista, pero habría que reescribirlo con punteros; además, el vector guarda los datos juntos en memoria y se recorre más rápido.

**Cola de reproducción: lista doblemente ligada (`DList<Song>`).** La cola necesita ir a la siguiente canción, regresar a la anterior y quitar la actual; `PlaybackQueue` también permite agregar una canción junto a la actual (comandos `a` e `i` de la cola):

| Operación | Vector | Lista simple | Lista doble |
|---|---|---|---|
| Siguiente | O(1) | O(1) | O(1) |
| Anterior | O(1) | O(n) | O(1) |
| Agregar después de la actual | O(n) | O(1) | O(1) |
| Agregar antes de la actual | O(n) | O(n) | O(1) |
| Quitar la actual | O(n) | O(n) | O(1) |

El vector mueve elementos para agregar o quitar en medio, y la lista simple no puede ir hacia atrás sin recorrer desde el inicio. La lista doble lo hace todo en O(1) porque cada nodo conoce al anterior y al siguiente. Con 1100 canciones la diferencia en tiempo real es poca, pero esta lista hace justo estas operaciones en tiempo constante aunque la biblioteca crezca.


**Lista de reproducción: `std::vector<Song>`.** Se arma agregando canciones al final, se muestra completa, se quita una por posición y se escribe completa al archivo. No necesita moverse hacia atrás y adelante, y el vector agrega al final en O(1) amortizado y se recorre rápido. Quitar una canción en medio cuesta O(p) por el desplazamiento, pero la lista es corta y esa operación es poco frecuente. Para reproducirla se copia a la misma `PlaybackQueue`, así que no necesito otra estructura de navegación.

**Cómo la uso:** la cola se arma con el vector ya ordenado. El cursor siempre apunta a una canción válida (en los extremos no se mueve, y si quitas la última regresa a la anterior). `DList` libera su memoria en el destructor.


## SICT0301: Evalúa los componentes

### Hace un análisis de complejidad correcto y completo para los algoritmos de ordenamiento usados en el programa

Solo se usa Merge Sort. Sea n el número de canciones.

- **Peor caso:** la recursión tiene siempre ⌈log₂(n)⌉ niveles y en cada nivel las mezclas hacen a lo sumo n comparaciones. Es **O(n log n)**.
- **Mejor caso:** aunque la lista ya esté ordenada, igual se divide y se mezcla en cada nivel. Solo se hacen menos comparaciones (cerca de la mitad). Es **Ω(n log n)**.
- **Caso promedio:** el trabajo por nivel no depende del orden de los datos, así que queda igual que los otros dos: **Θ(n log n)**.
- **Espacio:** `aux` es un vector de tamaño n que se crea una sola vez, más O(log n) de pila de la recursión. Es **O(n)**.

*Nota: al ordenar por texto se compara en minúsculas y el costo de cada comparación depende del largo de la cadena. Como son textos cortos, lo tomo como constante.*

### Hace un análisis de complejidad correcto y completo de todas las estructuras de datos y cada uno de sus usos en el programa

Sea n el número de canciones y F el número de archivos y carpetas dentro de la carpeta de música.

**`std::vector<Song>` (la biblioteca)**

| Operación | Mejor caso | Caso promedio | Peor caso |
|---|---|---|---|
| Agregar una canción con `push_back` (al leer la carpeta) | O(1) | O(1) amortizado | O(n) (el vector se llenó y copia todo a un espacio más grande) |
| Agregar las n canciones al leer la carpeta | Θ(n) | Θ(n) | Θ(n) (el costo amortizado de cada `push_back` es constante) |
| Entrar a una canción por posición (`mergeRanges`, `songs[index]`) | O(1) | O(1) | O(1) |
| Crear el vector auxiliar (`aux`) | Θ(n) | Θ(n) | Θ(n) |
| Ordenar (`mergeSort`) | Θ(n log n) | Θ(n log n) | Θ(n log n) |
| Recorrer e imprimir (`printSongs`) | Θ(n) | Θ(n) | Θ(n) |
| Buscar por título, artista o álbum (`findSongs`) | Θ(n) | Θ(n) | Θ(n) |

`findSongs` siempre revisa las n canciones porque devuelve todas las coincidencias: no puede detenerse antes aunque encuentre la primera. En cada canción compara a lo más tres textos (título, artista y álbum), que son cortos, así que ese trabajo es constante. Por eso sus tres casos son iguales. Lo único que cambia es cuántas coincidencias guarda, de 0 a n, y eso también es O(n) en total.


**`DList<Song>` (la cola)**

Cada nodo guarda el valor y dos enlaces (al anterior y al siguiente). El espacio es O(n).

| Operación | Mejor caso | Caso promedio | Peor caso |
|---|---|---|---|
| `add`, `addFirst`, `removeFirst`, `removeLast`, `getFirst`, `getLast`, `length`, `empty` | O(1) | O(1) | O(1) |
| `addBefore`, `addAfter`, `removeCurrent` (con iterador) y el iterador `++` / `--` | O(1) | O(1) | O(1) |
| `get`, `set`, `remove` por posición | O(1) (posición en un extremo) | O(n) (en promedio se recorren cerca de n/4 nodos) | O(n) (posición en el centro: n/2 nodos) |
| `contains`, `indexOf`, `removeFirstOcurrence` | O(1) (está en la primera posición) | O(n) (si está, en promedio cerca de n/2 comparaciones) | O(n) (está al final o no está) |
| `lastIndexOf`, `removeLastOcurrence` | O(1) (está en la última posición) | O(n) | O(n) |
| `clear`, destructor, copiar (`DList(const DList&)`, `operator=`), `toString` | Θ(n) | Θ(n) | Θ(n) |

`get`, `set` y `remove` por posición entran desde el extremo más cercano (`linkAt`), por eso el mejor caso es O(1); aun así, en promedio y en el peor caso siguen siendo O(n). `clear`, copiar y `toString` siempre pasan por todos los nodos, por eso sus tres casos coinciden.


**`PlaybackQueue`**

| Operación | Mejor caso | Caso promedio | Peor caso |
|---|---|---|---|
| `load` (copia el vector a la lista) | Θ(n) | Θ(n) | Θ(n) |
| `empty`, `length`, `position`, `current`, `next`, `previous`, `addAfter`, `addBefore`, `removeCurrent` | O(1) | O(1) | O(1) |

**Lista de reproducción (`std::vector<Song>`, funciones de `main.cpp`)**

Sea p el número de canciones de la lista y m el número de coincidencias de una búsqueda (m ≤ n).

| Operación | Mejor caso | Caso promedio | Peor caso |
|---|---|---|---|
| `containsSong` (revisa si la ruta ya está en la lista) | O(1) (está en la primera posición) | O(p) | O(p) (está al final o no está) |
| Agregar una canción (`containsSong` + `push_back`) | O(1) (lista vacía) | O(p) | O(p) |
| Buscar candidatas para agregar (`findSongs` + imprimir las m coincidencias) | Θ(n) | Θ(n + m) | Θ(n + m) |
| Quitar una canción (`erase` por posición) | O(1) (la última) | O(p) (se desplazan en promedio p/2) | O(p) (la primera) |
| Mostrar la lista (`printPlaylist`) | Θ(p) | Θ(p) | Θ(p) |
| Guardar como `.m3u8` (`savePlaylistM3u8`, una línea por canción) | Θ(p) | Θ(p) | Θ(p) |
| Listar los `.m3u8` para elegir uno (`openPlaylist`: `directory_iterator` sobre la carpeta + `std::sort` de los f nombres) | Θ(F) + O(f log f) | Θ(F) + O(f log f) | Θ(F) + O(f log f) |
| Abrir un `.m3u8` (`loadPlaylistM3u8`: por cada una de las p líneas se busca su ruta en la biblioteca) | Θ(p) (cada ruta está al inicio de la biblioteca) | Θ(n · p) (en promedio se revisa cerca de la mitad de la biblioteca por línea) | Θ(n · p) (las rutas están al final o no existen) |
| Reproducir la lista (`PlaybackQueue::load` + comandos) | Θ(p) para cargar, O(1) por comando | Θ(p), O(1) por comando | Θ(p), O(1) por comando |

La búsqueda (`findSongs`) revisa las n canciones siempre, por eso su costo es Θ(n); imprimir los m resultados agrega Θ(m), y como m ≤ n el total sigue siendo Θ(n).

En `loadPlaylistM3u8`, p es el número de líneas del archivo. Como cada línea se busca de forma lineal en la biblioteca, el costo es Θ(n · p); si la lista tiene del orden de n canciones, eso llega a Θ(n²). En `openPlaylist`, f es el número de playlists que hay en la carpeta (f ≤ F).

**Costo de cada opción del menú**

| Opción | Costo (los tres casos) |
|---|---|
| Elegir el criterio de orden al inicio, o la opción 2 (`mergeSort`) | Θ(n log n) |
| 1. Mostrar la biblioteca (`printSongs`) | Θ(n) |
| 3. Buscar una canción (`findSongs` + imprimir) | Θ(n) por cada búsqueda |
| 4. Reproducir la cola (`load` con toda la biblioteca) | Θ(n) para cargar, O(1) por comando (play/pausa, `s`, `n`, `b`, `r`) |
| 5. Lista de reproducción | las de la tabla anterior (la más cara es abrir un `.m3u8`: Θ(n · p)) |
| Cualquier texto o número que escribe el usuario (`numberFromInput`, `criterionFromInput`, `actionFor`) | O(1) (son textos cortos) |


**Reproducción de audio (`playQueue` y `audio.cpp`)**

Dentro de la cola, el programa espera dos cosas a la vez: lo que escribe el usuario y el final de la canción. La línea del usuario se lee en un segundo hilo (`std::async` con `std::getline`) y el hilo principal revisa cada 100 ms si ya llegó (`wait_for`). Si no llegó y algo está sonando, pregunta a SDL si la canción terminó (`finished`) y, si terminó, pasa a la siguiente. Así el avance automático funciona sin que el usuario tenga que interactuar.

| Acción | Costo (los tres casos) |
|---|---|
| `p` al empezar (armar la ruta con `startSong` + abrir y empezar el archivo en SDL) | O(1) de parte del programa; abrir el archivo lo hace SDL |
| Pausar, continuar y `s` (detener) | O(1) |
| `n`, `b` y `r` (mueven el cursor o quitan la actual) y empezar la nueva canción si algo sonaba | O(1) |
| Redibujar la pantalla tras cada comando (`clearScreen` + una línea de estado) | O(1) |
| Cada revisión de 100 ms (¿la canción terminó?) | O(1) |
| Avance automático al terminar una canción | O(1) por canción |
| Salir de la cola (`stop`) | O(1) |
| `a` o `i` (buscar con `findSongs` en la biblioteca + `addAfter` / `addBefore`) | Θ(n) por la búsqueda; agregar a la cola es O(1) 

El audio no agrega memoria que crezca con n: SDL tiene abierta una sola canción a la vez y `stop` libera la anterior antes de abrir la siguiente.

La decodificación de FLAC y Opus la hace SDL2_mixer. Para el programa, empezar una canción es una llamada, así que su costo propio es O(1) y el costo de decodificar entra en el análisis.

**Complejidad final del programa.**

- **Al arrancar:** leer la carpeta cuesta Θ(F) y ordenar Θ(n log n), así que el arranque es **Θ(F + n log n)**.
- **Después, la acción más cara** es abrir un `.m3u8` de p líneas, **Θ(n · p)** en el caso promedio y en el peor (llega a Θ(n²) si la lista tiene del orden de n canciones). Le sigue cambiar el criterio de orden, **Θ(n log n)**.
- **Las demás acciones** son Θ(n) o menos: mostrar la biblioteca, buscar y cargar la cola cuestan Θ(n); mostrar y guardar una playlist cuestan Θ(p); listar los `.m3u8` cuesta Θ(F) + O(f log f). Los comandos de la cola son O(1).
- **Espacio:** **O(n + p)**: el vector de la biblioteca (n), la cola (hasta n nodos), la lista de reproducción (p), la lista que se arma al abrir un `.m3u8` (p) y el vector auxiliar de Merge Sort (n), que se libera al terminar de ordenar.

Reproducir audio no cambia esta cuenta: cada comando de la cola, cada revisión de fin de canción y cada avance automático cuestan O(1).

## SICT0303: Implementa acciones científicas

### Implementa mecanismos para consultar información de las estructuras correctos

- **Biblioteca:** el usuario elige cómo ordenar (título, artista, álbum, año o pista) y `printSongs` la muestra en ese orden. Una canción por posición se consulta con `songs[i]`, en O(1).
- **Búsqueda:** `findSongs` regresa las posiciones de todas las canciones cuyo título, artista o álbum contiene el texto escrito, sin importar mayúsculas o minúsculas. Con esas posiciones se consulta cada canción en O(1) y se muestran los resultados, tanto en la opción de buscar como al elegir qué canciones agregar a la lista.
- **Lista de reproducción:** `printPlaylist` la muestra completa y `containsSong` consulta si una canción ya está en ella.
- **Cola (`PlaybackQueue`), todo en O(1):** `current()` da la canción actual, `position()` en qué lugar va, `length()` cuántas hay, `empty()` si está vacía, y `next()` / `previous()` mueven el cursor (regresan `false` si ya estaba en un extremo). Con esto el programa muestra en cada paso la canción actual, su posición y cuántas quedan.
- `DList` también tiene `get`, `getFirst`, `getLast`, `contains`, `indexOf` y `lastIndexOf`, pero no las uso.

### Implementa mecanismos de lectura de archivos para cargar datos a las estructuras de manera correcta

La lectura está en `scanFolder` (`metadata.cpp`):

1. **Recorre la carpeta** y todas sus subcarpetas con `recursive_directory_iterator`.
2. **Filtra:** solo toma archivos `.flac` u `.opus` (sin importar mayúsculas o minúsculas). Los demás se ignoran.
3. **Lee los datos** de cada archivo con `TagLib::FileRef`: título, artista, álbum, año, pista y duración. En Windows usa la ruta como `wstring` para que funcionen los nombres con acentos.
4. **Si faltan datos** (no hay etiquetas o el título está vacío), usa el nombre del archivo como título y deja lo demás vacío o en 0. Así no se pierde ninguna canción.
5. **Guarda:** cada `Song` se agrega al vector con `push_back`. Ya ordenado, el vector se copia a la cola con `PlaybackQueue::load`.

Costo: O(F) para recorrer y leer los archivos, más O(n) para llenar el vector y O(n) para llenar la lista.

**Leer una lista de reproducción** (`loadPlaylistM3u8`, en `playlist.cpp`): sirve para volver a cargar un `.m3u8` guardado y seguir editándolo.

1. **Abre el archivo** con `std::ifstream`. Si no se puede abrir, regresa `false` y el programa avisa.
2. **Lee línea por línea.** Quita el salto de línea de Windows (`\r`). Ignora las líneas vacías y las que empiezan con `#` (el encabezado `#EXTM3U` y los comentarios).
3. **Normaliza la ruta:** cambia `\` por `/` y quita el `./` del inicio, para que quede igual que `Song::path`.
4. **Busca la canción** en la biblioteca comparando la ruta con `Song::path`. Si la encuentra, copia el `Song` a la lista cargada (en el orden del archivo); si no, cuenta la línea como no encontrada. Así una lista guardada con otra carpeta o con canciones que ya no existen no rompe el programa.
5. **Elegir el archivo:** el menú recorre la carpeta de música con `directory_iterator`, muestra los `.m3u8` o `.m3u` ordenados por nombre y el usuario escoge uno por número.

Costo: se leen las p líneas del archivo y cada una se busca en la biblioteca. Es **Θ(p)** en el mejor caso (cada ruta está al inicio) y **Θ(n · p)** en el promedio y en el peor caso.

El archivo de audio en sí se abre hasta que el usuario escribe `p` (o `play`) por primera vez: `startSong` une la carpeta de música con `Song::path` (que es relativa a ella) y se lo pasa a SDL2_mixer (`Mix_LoadMUS`). Si el archivo no existe o no se puede decodificar, el programa muestra el motivo y se queda detenido en esa canción.

### Implementa mecanismos de escritura de archivos para guardar los datos de las estructuras de manera correcta

La escritura está en `savePlaylistM3u8` (`playlist.cpp`), que guarda el contenido del `std::vector<Song>` de la lista de reproducción en un archivo `.m3u8`:

1. **Abre el archivo** con `std::ofstream`. Si no se puede abrir, regresa `false` y el programa avisa con el nombre completo del archivo en lugar de decir que se guardó.
2. **Escribe el encabezado** `#EXTM3U`, que es el formato estándar de las listas M3U.
3. **Escribe una línea por canción** con su ruta, en el orden de la lista. Las rutas son relativas a la carpeta de música (`Song::path`) y llevan `./` al inicio, por ejemplo `./Artista/[2026] Álbum/01. Canción.flac`.
4. **Verifica** con `file.good()` que no hubo errores al escribir.

El programa guarda el archivo dentro de la carpeta de música, porque las rutas dentro del archivo son relativas a esa carpeta; si se guardara en otro lugar, otro reproductor no encontraría las canciones.

Costo: Θ(p) para una lista de p canciones.

## SEG0701: Cultura digital

### El producto genera valor para un usuario final y se ejecuta sin errores

**Qué problema resuelve.** Quien tiene su música en archivos FLAC y Opus necesita ordenarla, encontrar una canción, armar listas y escucharlas. El programa hace todo eso desde la terminal con la propia carpeta de música: no copia ni modifica los archivos, y las listas se guardan en `.m3u8`, un formato que también abren otros reproductores.

**Cómo evita errores.** El programa revisa lo que puede salir mal en lugar de cerrarse:

- Si la ruta no existe, no es una carpeta o no se puede leer, lo avisa y termina con un mensaje (no se cae).
- Si no hay dispositivo de audio o SDL2_mixer no soporta FLAC u Opus, lo avisa al inicio y se puede seguir usando todo lo demás.
- Si un archivo no se puede reproducir, muestra el motivo y se queda en esa canción.
- Si un número, una opción o un comando no es válido, lo dice y vuelve a preguntar.
- Si una playlist está vacía, o se intenta abrir un `.m3u8` con canciones que ya no existen, el programa lo avisa y omite esas líneas.
- Si no se puede escribir el `.m3u8`, el mensaje lo dice con el nombre completo del archivo, en lugar de decir que se guardó.

El programa no me ha dado ningún error en las pruebas que hice. Cabe aclarar que todas las pruebas las hice en Windows y con MSYS2 agregado en el PATH, por lo que realmente no estoy seguro de cómo funcionaría en otros sistemas operativos. Teóricamente debería funcionar en Linux y en Mac.
