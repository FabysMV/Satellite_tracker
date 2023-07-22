<h1 align="center"> Satellite tracker </h1>
<p align="left">
   <img src="https://img.shields.io/badge/STATUS-EN%20DESAROLLO-green">
   </p>
El control de este seguidor se basa en un PID cuyo sistema de retroalimentación es un sensor IMU MPU9050.
Los valores del IMU para el <b>roll</b> corresponden al <b>cenit </b>del tracker y estos van <b>desde 0° hasta 180° aproximadamente</b>. 
Cuando el motor gira en sentido horario (visto desde el frente, es decir, del lado de los engranes), los valores del IMU decrementan, 
en caso contrario, si se realiza el giro en sentido antihorario, los valores incrementarán.
<br>Para el <b>yaw</b> los valores corresponden al <b>azimut</b> del tracker.
Si se gira en sentido horario (visto desde arriba) los valores aumentan, si gira en sentido antihorario los valores decrementan. <b>Los valores van desde 0° hasta 360°</b>

<br><br>
:construction: <b>Se corrigieron errores en la versión 2.3 y 3.0</b>:construction:
<br>
Al momento de iniciar el programa, los datos del sensor no se actualizan y se quedan estáticos en un valor de roll = 90 y yaw = 3. Se desconoce el origen de este error ya que
el sensor funciona correctamente con otro código que es exclusivo para leer los datos de éste.
<br>Además, a veces sí funciona y otras veces no. A veces puede leer y actualizar los datos y después de un tiempo se congelan y se quedan estáticos.

<br>En la versión 2.3 se utiliza el magnetómetro el IMU, puede ser algo inestable si éste no se calibra correctamente
<br><br>En la versión 3.0 se utiliza una librería para el IMU que toma un punto de referencia para Yaw, por lo que se inicializará en 359 automáticamente después de ser calibrado. No es inestable.
