# Lector de biblioteca musical (FLAC/Opus)

El programa lee tu música en FLAC y Opus con TagLib, la ordena por título, artista, álbum, año o número de pista, y arma una cola de reproducción para ir a la canción siguiente o a la anterior.

En este avance el programa:

1. Busca archivos FLAC y Opus en la carpeta que elijas (y en sus subcarpetas).
2. Lee sus datos con TagLib y los guarda en un `Song` dentro de un `std::vector`.
3. Ordena el vector con Merge Sort y lo imprime.
4. Copia las canciones ordenadas a una lista doblemente ligada (`DList`) que funciona como cola de reproducción.

Todavía no suena el audio, solo se navega por la cola y falta la interfaz grafica(me va a matar hacerla).


## Cómo usar el programa

### Requisitos

Un compilador de C++ con soporte para C++17 y la librería TagLib.

### Instalar dependencias

**Windows**

1. Instala MSYS2
2. Abre la terminal **MSYS2 MINGW64** (no la MSYS2 normal) y corre:

`pacman -Syu`

`pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-taglib mingw-w64-x86_64-pkg-config`

**Linux — Arch:** `sudo pacman -S base-devel taglib`

**Linux — Ubuntu/Debian:** `sudo apt install g++ libtag1-dev pkg-config`

**macOS** (con Homebrew): `brew install taglib pkg-config`

### Compilar

Deja en la misma carpeta: `main.cpp`, `metadata.cpp/.hpp`, `sorter.cpp/.hpp`, `song.hpp`, `dlist.h`, `playbackQueue.hpp` y `exception.h`.

**Linux / macOS / Windows (terminal MSYS2 MINGW64)**

`g++ -std=c++17 main.cpp metadata.cpp sorter.cpp -o lector $(pkg-config --cflags --libs taglib)`

**Windows (PowerShell, con MSYS2 ya agregado al PATH)**

`g++ -std=c++17 -IC:\msys64\mingw64\include main.cpp metadata.cpp sorter.cpp -o lector.exe -LC:\msys64\mingw64\lib -ltag`

### Ejecutar

`./lector "<ruta_a_tu_carpeta_de_musica>" [1-5]` (en Windows, `lector.exe`)

El segundo argumento es opcional. Si lo omites, el programa te muestra un menú para elegir cómo ordenar (1 título, 2 artista, 3 álbum, 4 año, 5 pista).

Después de imprimir la biblioteca se abre la cola, que muestra la canción actual y en qué lugar va (por ejemplo `2 of 5, 3 left`). Comandos: `n` siguiente, `p` anterior, `r` quitar la canción actual, `e` salir.

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

**Descarto Bubble Sort e Insertion Sort** por indicaciones del profesor y porque en promedio y en el peor caso son O(n²).

**Descarto Quick Sort** porque su peor caso es O(n²).

**Descarto Heap Sort** porque no es estable y en la práctica accede a la memoria de forma menos ordenada.

**Elijo Merge Sort** por dos razones:

1. **Siempre es O(n log n).** No importa cómo venga ordenada la biblioteca.
2. **Es estable.** Si dos canciones empatan (por ejemplo, del mismo año), se quedan en el orden que tenían. Así las canciones de un mismo álbum o artista no se revuelven.

*Nota: Merge Sort está en la función `mergeSort()` del archivo `sorter.cpp`. Los cinco criterios usan el mismo algoritmo; solo cambia la función de comparación (`comparatorFor`).*

### Selecciona una estructura de datos adecuada al problema y la usa correctamente

Uso dos estructuras, porque la biblioteca y la cola necesitan cosas distintas.

**Biblioteca: `std::vector<Song>`.** Se llena una vez, se ordena y se imprime. Para eso hay que agregar al final y entrar a cualquier posición en O(1), que es lo que necesita Merge Sort. Una lista no sirve aquí: entrar a una posición cuesta O(n) y ordenar subiría a O(n² log n).

**Cola de reproducción: lista doblemente ligada (`DList<Song>`).** La cola necesita ir a la siguiente canción, regresar a la anterior, agregar una junto a la actual y quitar la actual:

| Operación | Vector | Lista simple | Lista doble |
|---|---|---|---|
| Siguiente | O(1) | O(1) | O(1) |
| Anterior | O(1) | O(n) | O(1) |
| Agregar después de la actual | O(n) | O(1) | O(1) |
| Agregar antes de la actual | O(n) | O(n) | O(1) |
| Quitar la actual | O(n) | O(n) | O(1) |

El vector mueve elementos para agregar o quitar en medio, y la lista simple no puede ir hacia atrás sin recorrer desde el inicio. La lista doble lo hace todo en O(1) porque cada nodo conoce al anterior y al siguiente. Con 1100 canciones la diferencia en tiempo real es poca, pero esta lista hace justo estas operaciones en tiempo constante aunque la biblioteca crezca.

**Cómo la uso:** la cola se arma con el vector ya ordenado. El cursor siempre apunta a una canción válida (en los extremos no se mueve, y si quitas la última regresa a la anterior). `DList` libera su memoria en el destructor.

## SICT0301: Evalúa los componentes

### Hace un análisis de complejidad correcto y completo para los algoritmos de ordenamiento usados en el programa

Solo se usa Merge Sort. Sea n el número de canciones.

- **Peor caso:** la recursión tiene siempre ⌈log₂(n)⌉ niveles y en cada nivel las mezclas hacen a lo sumo n comparaciones. Es **O(n log n)**.
- **Mejor caso:** aunque la lista ya esté ordenada, igual se divide y se mezcla en cada nivel. Solo se hacen menos comparaciones (cerca de la mitad). Es **Ω(n log n)**.
- **Caso promedio:** el trabajo por nivel no depende del orden de los datos, así que queda igual que los otros dos: **Θ(n log n)**.
- **Espacio:** `aux` es un vector de tamaño n que se crea una sola vez, más O(log n) de pila de la recursión. Es **O(n)**.

### Hace un análisis de complejidad correcto y completo de todas las estructuras de datos y cada uno de sus usos en el programa

Sea n el número de canciones y F el número de archivos y carpetas dentro de la carpeta de música.

**`std::vector<Song>` (la biblioteca)**

- Agregar canciones al leer la carpeta (`push_back`): O(1) cada una, O(n) todas.
- Entrar a una canción por posición (`mergeRanges`): O(1).
- Crear el vector auxiliar y ordenar (`mergeSort`): O(n) y O(n log n).
- Recorrer e imprimir (`printSongs`): O(n).

**`DList<Song>` (la cola)**

Cada nodo guarda el valor y dos enlaces (al anterior y al siguiente). El espacio es O(n).

| Operaciones | Complejidad |
|---|---|
| `add`, `addFirst`, `removeFirst`, `removeLast`, `getFirst`, `getLast`, `length`, `empty` | O(1) |
| `addBefore`, `addAfter`, `removeCurrent` (con iterador) y el iterador `++` / `--` | O(1) |
| `get`, `set`, `remove` por posición (se recorre desde el extremo más cercano) | O(n) |
| `contains`, `indexOf`, `lastIndexOf`, `removeFirstOcurrence`, `removeLastOcurrence`, `clear`, copiar | O(n) |

**`PlaybackQueue`:** `load` (copia el vector a la lista) es O(n). Todo lo demás (`empty`, `length`, `position`, `current`, `next`, `previous`, `addAfter`, `addBefore`, `removeCurrent`) es O(1).

**Total del programa:** leer la carpeta O(F) + ordenar O(n log n) + imprimir O(n) + cargar la cola O(n). Cada comando de la cola es O(1). El tiempo total es **O(F + n log n)** y el espacio total es **O(n)**.

## SICT0303: Implementa acciones científicas

### Implementa mecanismos para consultar información de las estructuras correctos

- **Biblioteca:** el usuario elige cómo ordenar (título, artista, álbum, año o pista) y `printSongs` la muestra en ese orden. Una canción por posición se consulta con `songs[i]`, en O(1).
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
