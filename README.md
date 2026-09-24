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

Mi biblioteca de música cuenta con aproximadamente 1100 canciones en formatos FLAC u OPUS. Debido a la cantidad de archivos, decidí usar Insertion Sort, el cual tiene una complejidad promedio de $O(n^2)$, aunque en el mejor de los casos es $O(n)$. Decidí usar este algoritmo por una sencilla razón: me pareció el más eficiente entre los de menor complejidad de código. Por ejemplo, Bubble Sort tiene la misma complejidad teórica, pero en la práctica es mucho menos eficiente. Esto ocurre porque Insertion Sort detiene sus comparaciones inmediatamente en cuanto encuentra el lugar correcto para el elemento, mientras que un Bubble Sort continúa haciendo comparaciones e intercambios innecesarios por todo el arreglo.
Aunque para esta cantidad de archivos la diferencia sea imperceptible, si ambos requieren el mismo esfuerzo de programación, prefiero usar el más óptimo. En cuanto a otros algoritmos como Merge Sort o Quick Sort, me parece que su complejidad de implementación es excesiva para un organizador sencillo, ya que la ganancia en velocidad sería mínima. 

*Nota: Esta solución se encuentra implementada en la función `insertionSort()` dentro del archivo `sorter.cpp`.*

## SICT0301: Evalúa los componentes

### Hace un análisis de complejidad correcto y completo para los algoritmos de ordenamiento usados en el programa

**Peor caso**

Para que el ciclo interno (while) corra el máximo número de veces posible, las canciones tendrían que venir en orden inverso al que deseas organizarlas (por ejemplo, si quieres ordenar de la 'A' a la 'Z', las canciones deberian de estar de la 'Z' a la 'A'). Cada vez que el algoritmo evalúa una nueva canción, esta resulta ser la "menor" de todas las analizadas hasta ese momento, obligándola a recorrer todo el arreglo. En la posición `i`, el ciclo interno corre hasta `i` veces; sumando desde `i=1` hasta `n-1` se obtiene la progresión matemática $1+2+...+(n-1)$, la cual se resuelve como $n(n-1)/2$. 
Por lo tanto, el peor caso es **O(n²)**.

**Mejor caso**

El mejor caso seria que literalmente la lista de canciones ya estuviera ordenada. En esta situación, el algoritmo solo requeriría de realizar una comparación por cada canción (una comparación por cada posición `i`), dándose cuenta de que ya es mayor que la anterior sin entrar nunca al cuerpo del ciclo `while`. Esto suma un total de $n-1$ comparaciones. 
Por lo tanto, el mejor caso es **O(n)**.

**Caso promedio**

En una lista desordenada al azar, lo más normal es que una canción no tenga que retroceder hasta el principio (peor caso), ni que ya este completamente ordenada (mejor caso). En promedio, solo tendrá que retroceder la mitad del camino; es decir, el algoritmo tendría que hacer en total la mitad del trabajo que haría en el peor de los casos. Sin embargo, en análisis asintótico, "la mitad de un esfuerzo al cuadrado" sigue siendo un esfuerzo al cuadrado. 
Por lo tanto, el caso promedio también es **O(n²)**.

**Espacio**

El algoritmo implementado en `insertionSort()` no reserva ningún vector ni estructura nueva del tamaño de canciones. Simplemente utiliza variables temporales para realizar los intercambios dentro del mismo vector original. Esto implica que la cantidad de memoria extra utilizada es fija y no crece sin importar si tienes 10 o 10,000 canciones. 
Por lo tanto, el espacio adicional es **O(1)**.
