��    G      T  a   �        	     "     >   >     }     �  "   �  
   �     �  1  �       %   4  &   Z     �  /   �  '   �  -   �     	     -	  %   3	  M   Y	  "   �	  q   �	     <
     K
     Y
  &   s
  $   �
     �
     �
     �
     �
          !     *  '   B  &   j     �     �  $   �     �  .   �  �   ,  *   �  ^     8   g     �    �  .   �  �   �  	   ~     �     �     �     �     �     �     �     �     �     �       	   0     :     @     Y  	   ^  <   h     �     �     �  �  �     �  *   �  ?   �  &        C  $   U     z     �  D  �  "   �  ,     /   .     ^  )   w  !   �  (   �     �       4     W   I  '   �  q   �     ;     M      ^  *     +   �     �  %   �          +  -   @  	   n     x  &   �  *   �     �     �  &     -   <  2   j  �   �  0   v  s   �  E        a  $  �  3   �  �   �     ~     �     �     �     �     �     �     �     �     	       #   )     M     T     ]     x  
     K   �     �     �     �     E          (      )                 '   <                    
       8   ?                         &                	      ;   +      ,                     *   /   3   =      $      %      F   :          5   6   "       9   .      >   0                             G   #   1      !          4          C       @   D              A   -   2       B      7     (custom)  command not found, cannot proceed  couldn't be executed successfully. You must run this as root!  not found. Is grub2 installed? (new Entries) Add a script to your configuration Add script BURG found! Before you can edit your grub configuration we have to
mount the required partitions.

This assistant will help you doing this.


Please ensure the target system is based on the same cpu architecture
as the actually running one.
If not, you will get an error message when trying to load the configuration. Configuration has been saved Couldn't mount the selected partition Couldn't umount the selected partition Default title:  Do you want to configure BURG instead of grub2? Do you want to save your modifications? Do you want to select another root partition? Edit grub preferences Entry Error while installing the bootloader Grub Customizer is a graphical interface to configure the grub2/burg settings Grub Customizer: Partition chooser Install the bootloader to MBR and put some
files to the bootloaders data directory
(if they don't already exist). Install to MBR Mount failed! Mount selected Filesystem Move down the selected entry or script Move up the selected entry or script Name the Entry No Bootloader found No script found Partition Chooser Please type a device string! Preview: Proxy binary not found! Remove a script from your configuration Save configuration and generate a new  Script to insert: Select _partition … Select and mount your root partition Select required submountpoints The bootloader has been installed successfully The generated configuration didn't equal to the saved configuration on startup. So what you see now may not be what you see when you restart your pc. To fix this, click update! The saved configuration is not up to date! These are the mountpoints of your fstab file.
Please select every grub/boot related partition. This seems not to be a root file system (no fstab found) Unmount mounted Filesystem You selected the option for choosing another partition.
Please note: The os-prober may not find your actually running system.
So run Grub Customizer on the target system
again and save the configuration (or run update-grub/update-burg)
to get the entry back! You started Grub Customizer using the live CD. You will see all entries (uncustomized) when you run grub. This error accurs (in most cases), when you didn't install grub gustomizer currectly. _Device:  _Edit _File _Help _Install to MBR … _Quit without saving _Quit without update _Save & Quit _Update & Quit _View _use another partition:  installing the bootloader… is active label loading configuration… name partition reload configuration (same effect as an application restart) type umount failed! updating configuration Project-Id-Version: grub-customizer
Report-Msgid-Bugs-To: FULL NAME <EMAIL@ADDRESS>
POT-Creation-Date: 2010-10-22 20:01+0200
PO-Revision-Date: 2010-11-06 15:50+0000
Last-Translator: Bernardo Miguel Savone <Unknown>
Language-Team: Spanish
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
X-Launchpad-Export-Date: 2010-11-16 19:12+0000
X-Generator: Launchpad (build Unknown)
X-Poedit-Country: SPAIN
X-Poedit-Language: Spanish
  (personaliza)  comando no encontrado, imposible proceder  no se puede ejecutar con éxito. Debe ejecutar esto como root!  no encontrado. Tiene grub2 instalado? (nuevas Entradas) Agrega un script a tu configuración Agrega script BURG encontrado! Antes de modificar la configuración del grub es necesario
montar la partición requerida.

Este asistente te ayudará a hacerlo.


Asegúrate de que el sistema este basado en la misma arquitectura cpu
que el que se encuentra en ejecución.
Si no es así , obtendrás un mensaje de error mientras se carga la configuración. La configuración ha sido guardada No se pudo montar la partición seleccionada No se pudo desmontar la partición seleccionada Título predeterminado:  Deseas configurar BURG en lugar de grub2? Desea guardar las modificaciones? Deseas seleccionar otra partición root? Editar las preferencias de GRUB Entrada Error durante la instalación del gestor de arranque Grub Customizer es una interfaz gráfica para configurar las preferencias de GRUB2/BURG Grub Customizer: Seleccionar partición Instala el gestor de arranque en el MBR y pone algunos
archivos en el directorio del mismo
(si estos no existen). Instala en el MBR Montaje fallido! Monta el Filesystem seleccionado Mueve hacia abajo el elemento seleccionado Mueve hacia arriba el elemento seleccionado Nombre de la entrada Ningún gestor de arranque encontrado Ningún script encontrado Elegir la partición Por favor, escribe el nombre del dispositivo! Anteprima Proxy binario no encontrado! Elimina un script de tu configuración Guarda la configuración y crea una nueva  Script a insertar: Selecciona _partición ... Selecciona y monta tu partición raíz Selecciona otros puntos de montaje requeridos El gestor de arranque ha sido instalado con éxito La configuración generada no  es igual a la configuración guardada en el inicio. Así que lo que ves ahora puede no ser lo que se verá cuando reinicie el PC. Para solucionar este problema, haga clic en actualizar! La configuración guardada no está actualizada! Estos son los puntos de montaje de tu archivo fstab.
Por favor selecciona cada partición relacionada al grub/boot. Esto no parece ser un sistema de archivos raíz (fstab no encontrado) Desmonta el Filesystem montado Has seleccionado la opción para elegir otra partición.
Atención: os-prober podría no encontrar tu actual sistema en ejecución..
Por lo tanto debes reiniciar Grub Customizer en el sistema
y guardar la configuración (o ejecutar update-grub/update-burg)
para obtener la entrada nuevamente! Has iniciado Grub Customizer utilizando el live CD. Verás todas las entradas (no personalizadas) al arrancar grub. Este error se produce (en la mayoría de los casos) si no se instaló grub customizer correctamente. _Dispositivo  _Editar _Archivo _Ayuda _Instalar en el MBR... _Salir sin guardar _Salir sin actualizar _Guardar y salir _Actualizar y salir _Ver _Utiliza otra partición:  instalando el gestor de arranque... activa etiqueta cargando configuración... nombre partición recargar la configuración (mismo efecto que un reinicio de la aplicación) tipo falló al desmontar! actualizando la configuración 