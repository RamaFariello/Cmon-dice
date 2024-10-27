# Cmon-dice
## Instrucciones de Juego y cuestiones para tener en cuenta
- A: Iniciar Juego
- B: Salir

## Ingreso de nombres
Al iniciar el juego, deberá cargar los nombres de los jugadores utilizando solo letras y espacios, tal como indica el formato en pantalla. Para ingresar otro jugador presione la tecla ENTER.
Una vez que haya ingresado todos los nombres, presione ‘X’.

## Selección de dificultad
Podrá seleccionar la dificultad de juego (aplicable a todos los jugadores) pulsando una de las teclas:
-	F: Fácil
-	M: Medio 
-	D: Difícil <br/>

Según la dificultad seleccionada se cargarán los valores correspondientes de cantidad de vidas, tiempo de visualización y tiempo de respuesta, cargados en el archivo config.txt.
Podrá ver los valores seteados en pantalla una vez elegida la dificultad.

## Configurar niveles de dificultad (config.txt)
Para determinar los valores de los niveles a gusto, deberá editar el archivo config.txt.
El formato a seguir es el siguiente:
INICIAL_NIVEL|TIEMPO_DE_VISUALIZACION|TIEMPO_DE_RESPUESTA|VIDAS
-	No debe haber espacios.
-	Los campos deben estar separados únicamente por pipes (‘|’).
-	Cada nivel está separado por un salto de línea.
-	Las iniciales de niveles deben ser: F, M o D.
-	Los tiempos, tanto de visualización como de respuesta, deben ser superiores a 0 y menores o iguales a 20 segundos.
-	Las vidas deben ser mayores o iguales a 0 y menores o iguales a 5.
-	Para que el juego funcione, debe haber una configuración para cada nivel de dificultad (fácil, medio y difícil).

### Ejemplo de configuración:
F|20|15|5 <br/>
M|15|10|2 <br/>
D|3|2|0 <br/>

## Orden de jugadores
Una vez seleccionada la dificultad, se mostrará el orden aleatorio de los jugadores.

## Juego
### Los caracteres válidos para ingresar durante el juego son:
- X: uso de vida.
- V: color verde.
- A: color amarillo.
- R: color rojo.
- N: color naranja.

### Cómo jugar
Se le mostrará al jugador la secuencia asignada durante el tiempo determinado por la dificultad. Una vez finalizado el conteo, tendrá un tiempo determinado para ingresar la respuesta correcta, replicando esa secuencia.
> Ejemplo: Si se le asignó la secuencia V-A-R-N, deberá memorizar y volver a ingresar esa misma secuencia, en el mismo orden, antes de que se le acabe el tiempo.

- A medida que responde correctamente, la secuencia irá incrementando en cantidad de caracteres.
- Por cada respuesta correcta, se le sumará 1 segundo extra de visualización y de respuesta.

### Uso de vidas
- El jugador podrá utilizar vidas, apretando la tecla ‘X’, para retroceder posiciones en su respuesta ingresada. Puede retroceder tantas posiciones como vidas tenga.
- Si se han ingresado N caracteres de secuencia y se desean utilizar N+1 vidas, se le mostrará nuevamente la secuencia asignada al jugador. Caso contrario, no se mostrará la secuencia y el jugador deberá continuar ingresando los caracteres desde la posición en la que quedó.
- Si el jugador no ha ingresado ningún caracter y se le ha agotado el tiempo: se mostrará la secuencia asignada nuevamente.
- Si el jugador no ha ingresado ningún caracter y presiona la tecla ‘X’: se mostrará la secuencia asignada nuevamente.

### Fin del juego
Si el jugador se equivoca y ya no le quedan vidas, termina su ronda y se pasa al siguiente jugador.<br/>
Una vez que todos los jugadores hayan tenido su turno se mostrará el informe y los ganadores en pantalla. También se generará un archivo informe.txt con la misma información.


## Integrantes del grupo:
Este proyecto ha sido elaborado por: 
 - Brocani, Agustín
 - Fariello, Ramiro
 - Monges, Omar
 - Odriozola, Ignacio
 - Quiroga Piegari, Lucila
 - Sierra, Jazmín
