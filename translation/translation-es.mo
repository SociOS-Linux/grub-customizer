��    {      �  �   �      h
  	   i
  8   s
  !   �
     �
     �
     �
  E   �
     D  "   P  
   s  	   ~     �  1  �     �  %   �  &   	     0  /   @  '   p  -   �     �     �     �  %   �  M     "   m  q   �                 &   9  $   `     �     �     �     �     �     �     �  '     &   0     W     i     q  $   �     �  .   �  �   �  *   �  ^   �  �   5  8   �       G   
     R    m  .   p  �   �  	   0  	   :     D     J     P     V     _     e     y     �     �     �     �     �     �  
   �     	                %     +     B     V  	   [     e  
   s     ~     �  
   �     �  	   �     �     �  
   �  
   �  
   �     	       	   #     -      F     g     o     {     �  	   �     �     �     �     �     �     �  *   �          !  	   *     4     @     E     T     k  
   q     |     �  �  �     X  @   i  )   �     �     �     �  V        i  %   u     �  	   �     �  H  �  "     ,   0  /   ]     �  +   �  $   �  +   �     #     C     K  4   i  W   �  '   �  q        �     �      �  *   �  +   �     +   *   =      h      �   "   �      �      �   &   �   *   !     6!     I!     R!  &   n!  +   �!  2   �!  �   �!  /   �"  i   �"  �   _#  E   $     I$  T   Z$     �$    �$  2   �%  �   &  	   �&     �&     �&     �&     �&     �&     �&     '     '     +'     A'     R'     f'     k'      �'     �'     �'     �'     �'     �'     �'     �'     	(     (     (     1(  !   F(     h(  
   n(  #   y(     �(     �(     �(  
   �(  
   �(  
   �(     �(     �(  
   �(     
)      %)     F)     N)     _)     f)  
   n)     y)     �)     �)     �)     �)     �)  )   �)     *     *     (*     6*     C*     H*     \*     {*     �*     �*     �*         N   D   3   h       =   0   g   Y   C              .   Z                      l   w                      S      ,       y       :   5   U   ;   @   4   `       (   2   )       x         8      e       j   /           f   9      >   K             F       n   E   V                 	   c              m      1                   Q           O   [   %       o      u   +   d   t   
   R          H   z   b   W   L       r          '   !          a   _   J         A      B       ?   i   v          6      -   I       G   s          ^   X       <          $       7       q   T   k       ]      *      &   p   P   \   #      M   "   {     (custom) %1 couldn't be executed successfully. error message:
 %2 %1 not found. Is grub2 installed? (new Entries of %1) (new Entries) (script code) AND: your modifications are still unsaved, update will save them too! A_ppearance Add a script to your configuration Add script BURG Mode BURG found! Before you can edit your grub configuration we have to
mount the required partitions.

This assistant will help you doing this.


Please ensure the target system is based on the same cpu architecture
as the actually running one.
If not, you will get an error message when trying to load the configuration. Configuration has been saved Couldn't mount the selected partition Couldn't umount the selected partition Default title:  Do you want to configure BURG instead of grub2? Do you want to save your modifications? Do you want to select another root partition? Edit grub preferences Entry Entry %1 (by position) Error while installing the bootloader Grub Customizer is a graphical interface to configure the grub2/burg settings Grub Customizer: Partition chooser Install the bootloader to MBR and put some
files to the bootloaders data directory
(if they don't already exist). Install to MBR Mount failed! Mount selected Filesystem Move down the selected entry or script Move up the selected entry or script Name the Entry No Bootloader found No script found Partition Chooser Please type a device string! Preview: Proxy binary not found! Remove a script from your configuration Save configuration and generate a new  Script to insert: Seconds Select _partition … Select and mount your root partition Select required submountpoints The bootloader has been installed successfully The generated configuration didn't equal to the saved configuration on startup. So what you see now may not be what you see when you restart your pc. To fix this, click update! The saved configuration is not up to date! These are the mountpoints of your fstab file.
Please select every grub/boot related partition. This option doesn't work when the "os-prober" script finds other operating systems. Disable "%1" if you don't need to boot other operating systems. This seems not to be a root file system (no fstab found) Timeout To get the colors above working,
you have to select a background image! Unmount mounted Filesystem You selected the option for choosing another partition.
Please note: The os-prober may not find your actually running system.
So run Grub Customizer on the target system
again and save the configuration (or run update-grub/update-burg)
to get the entry back! You started Grub Customizer using the live CD. You will see all entries (uncustomized) when you run grub. This error accurs (in most cases), when you didn't install grub gustomizer currectly. _Advanced _Device:  _Edit _File _Font _General _Help _Install to MBR … _Quit without saving _Quit without update _Save & Quit _Update & Quit _View _use another partition:  add this entry to a new submenu background background image black blue brown cannot move this entry custom resolution:  cyan dark-gray default entry font color generate recovery entries green highlight: installing the bootloader… is active kernel parameters label light-blue light-cyan light-gray light-green light-magenta light-red loading configuration… look for other operating systems magenta menu colors name normal: partition pre_defined:  previously _booted entry red reload configuration remove background remove font remove this entry from the current submenu saved settings show menu transparent type umount failed! updating configuration value visibility white yellow Project-Id-Version: grub-customizer
Report-Msgid-Bugs-To: FULL NAME <EMAIL@ADDRESS>
POT-Creation-Date: 2012-06-20 00:16+0200
PO-Revision-Date: 2012-07-02 19:28+0000
Last-Translator: Eduardo Alberto Calvo <Unknown>
Language-Team: Spanish
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
X-Launchpad-Export-Date: 2012-08-30 21:28+0000
X-Generator: Launchpad (build 15877)
X-Poedit-Country: SPAIN
X-Poedit-Language: Spanish
  (personalizado) %1 no se pudo ejecutar satisfactoriamente. mensaje de error:
 %2 %1 no encontrado. Está instalado grub2 ? (entradas nuevas de %1) (nuevas entradas) (código de script) Y: las modificaciones no han sido guardadas aún, actualizar salvará éstas también. A_pariencia Añadir un script a su configuración Añadir script Modo BURG BURG encontrado Antes de modificar la configuración del grub es necesario
montar la partición requerida.

Este asistente le ayudará a hacerlo.


Asegúrese de que el sistema esté basado en la misma arquitectura de CPU
que el que se encuentra en ejecución.
Si no es así, obtendrá un mensaje de error al tratar de cargar la configuración. La configuración ha sido guardada No se pudo montar la partición seleccionada No se pudo desmontar la partición seleccionada Título predeterminado:  ¿Quiere configurar BURG en lugar de grub2? ¿Quiere guardar las modificaciones? ¿Quiere seleccionar otra partición raíz? Editar las preferencias de GRUB Entrada Entrada %1 (por la posición) Error durante la instalación del gestor de arranque Grub Customizer es una interfaz gráfica para configurar las preferencias de GRUB2/BURG Grub Customizer: Seleccionar partición Instala el gestor de arranque en el MBR y pone algunos
archivos en el directorio del mismo
(si estos no existen). Instalar en el MBR Montaje fallido Monta el Filesystem seleccionado Mueve hacia abajo el elemento seleccionado Mueve hacia arriba el elemento seleccionado Nombre la entrada No se encontró ningún gestor de arranque Ningún script encontrado Elegir la partición Escriba el nombre del dispositivo. Vista previa: Proxy binario no encontrado Elimina un script de su configuración Guarda la configuración y crea una nueva  Script a insertar: Segundos Seleccionar _partición ... Seleccione y monte su partición raíz Seleccionar subpuntos de montaje requeridos El gestor de arranque ha sido instalado con éxito La configuración generada no  es igual a la configuración guardada en el inicio. Así que lo que ve ahora puede no ser lo que se verá cuando reinicie el PC. Para solucionar este problema, pulse Actualizar. La configuración guardada no está actualizada Estos son los puntos de montaje de su archivo fstab.
Seleccione cada partición relacionada al grub/boot. Esta opción no funciona cuando el script «os-prober» encuentra otros sistemas operativos. Deshabilite «%1» si no tiene que arrancar otros sistemas operativos. Esto no parece ser un sistema de archivos raíz (fstab no encontrado) Tiempo de espera Para obtener los colores elegidos arriba,
tiene que seleccionar una imagen de fondo. Desmonta el Filesystem montado Ha seleccionado la opción para elegir otra partición.
Atención: os-prober podría no encontrar su sistema en ejecución actual.
Reinicie Grub Customizer en el sistema objetivo
y guarde la configuración (o ejecute update-grub/update-burg)
para obtener la entrada nuevamente. Ha iniciado Grub Customizer utilizando el Live CD. Verá todas las entradas (sin personalizar) al arrancar grub. Este error se produce (en la mayoría de los casos) si no se instaló Grub Customizer correctamente. _Avanzado _Dispositivo:  _Editar _Archivo _Tipografía _General Ay_uda _Instalar en el MBR... _Salir sin guardar _Salir sin actualizar _Guardar y salir _Actualizar y salir _Ver _utilice otra partición:  añadir esta entrada al submenú fondo imagen de fondo negro azul marrón no se puede mover esta entrada personalizar resolución  cian gris-oscuro entrada predeterminada color de tipografía generar entradas de recuperación verde resaltado: instalando el gestor de arranque... activa Parámetros del kernel etiqueta azul-claro cian-claro gris-claro verde-claro magenta-claro rojo-claro cargando configuración... buscar otros sistemas operativos magenta colores de menú nombre normal: partición pre_determinado  entrada_booteada previamente rojo recargar configuración eliminar fondo Remover tipografia eliminar esta entrada del submenú actual guardado configuración mostrar menú transparente tipo falló al desmontar actualizando la configuración valor visibilidad blanco amarillo 