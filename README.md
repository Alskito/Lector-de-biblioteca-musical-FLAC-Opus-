# Lector de biblioteca musical (FLAC/Opus)
El programa lee los metadatos de tu biblioteca de música en FLAC y Opus usando TagLib, y permite ordenarla por título, artista, álbum, año o número de pista.

Justo ahora, este avance consiste en recorrer la carpeta seleccionada en búsqueda de archivos FLAC u OPUS. El programa lee los metadatos con TagLib y los guarda en un objeto llamado song, el cual se agrega a un vector. El programa repite este proceso con todos los archivos que encuentre y, después, los ordena usando Insertion Sort dependiendo del criterio que hayas elegido.
Por ahora, esa era la idea principal de este avance. Sinceramente, no estoy seguro de qué quiero hacer en un futuro; todo dependerá de qué requiera agregar en los próximos avances. Sin embargo, es muy probable que termine haciendo un reproductor de música. No tiene mucho que dejé los servicios de streaming y ahora utilizo una aplicación llamada MusicBee, la cual adoro, pero siento que es demasiado básica, por lo que probablemente termine creando un reproductor de música para mi uso personal.

## Cómo usar el programa

### Requisitos

Necesitas un compilador de C++ con soporte para C++17, y la librería TagLib instalada.

### Instalar dependencias

**Windows**

1. Instala MSYS2
2. Abre la terminal **MSYS2 MINGW64** (no la MSYS2 normal) y corre:
   
`pacman -Syu`

`pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-taglib mingw-w64-x86_64-pkg-config`

**Linux — Arch**

`sudo pacman -S base-devel taglib`

**Linux — Ubuntu/Debian**

`sudo apt install g++ libtag1-dev pkg-config`

**macOS** (con Homebrew)

`brew install taglib pkg-config`

### Compilar

Desde la carpeta del proyecto:

**Linux / macOS / Windows (terminal MSYS2 MINGW64)**

`g++ -std=c++17 main.cpp metadatos.cpp sorter.cpp -o lector $(pkg-config --cflags --libs taglib)`

**Windows (PowerShell, con MSYS2 ya agregado al PATH)**

`g++ -std=c++17 -IC:\msys64\mingw64\include main.cpp metadatos.cpp sorter.cpp -o lector.exe -LC:\msys64\mingw64\lib -ltag`

### Ejecutar

`./lector "<ruta_a_tu_carpeta_de_musica>" [1-5]`

(en Windows, `lector.exe` en vez de `./lector`)

El segundo argumento es opcional, si lo omites, el programa te muestra un menú y te pregunta como lo quieres ordenar.

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

**Descarto Bubble Sort e Insertion Sort** por indicaciones del profesor y porque su complejidad promedio y de peor caso es O(n²).

**Descarto Quick Sort** porque su peor caso es O(n²).

**Descarto Heap Sort** porque, aunque también garantiza O(n log n) y usa O(1) de espacio extra, no es estable y en la práctica accede a memoria de forma menos ordenada.

**Elijo Merge Sort** por estas razones:

1. **O(n log n) garantizado en todos los casos.** El tiempo no depende de cómo venga ordenada la biblioteca.
2. **Es estable.** Si dos canciones empatan en el criterio (por ejemplo, dos canciones del mismo año), conservan el orden relativo que tenían. Esto es útil en un reproductor, donde se espera que las canciones del mismo álbum o artista no se reacomoden al azar.

*Nota: Esta solución se encuentra implementada en la función `insertionSort()` dentro del archivo `sorter.cpp`.*

## SICT0301: Evalúa los componentes

### Hace un análisis de complejidad correcto y completo para los algoritmos de ordenamiento usados en el programa

**Peor caso**

La mezcla siempre recorre todos los elementos del rango, sin importar el orden inicial, y la profundidad de la recursión siempre es log₂(n). En el peor caso el número de comparaciones es a lo sumo n·log₂(n) − n + 1.
Por lo tanto, el peor caso es **O(n log n)**.

**Mejor caso**

Aunque la lista ya esté ordenada, el algoritmo igual divide hasta el caso base y vuelve a mezclar en cada nivel (copiando los n elementos por nivel). Lo único que mejora es el número de comparaciones (alrededor de la mitad), pero el trabajo total sigue creciendo como n log n.
Por lo tanto, el mejor caso también es **Ω(n log n)**, es decir, Θ(n log n).

**Caso promedio**

Como la estructura de la recursión y el trabajo por nivel no dependen del orden de los datos, el caso promedio queda entre el mejor y el peor caso, que son ambos n log n.
Por lo tanto, el caso promedio es **Θ(n log n)**.

**Espacio**

`mergeSort()` reserva un vector auxiliar `aux` de tamaño n una sola vez (no uno nuevo en cada llamada recursiva), lo que da O(n). Además, la recursión tiene profundidad log₂(n), lo que añade O(log n) de pila. El total es O(n) + O(log n).
Por lo tanto, el espacio adicional es **O(n)**.

