��    y      �  �   �      8
  	   9
  8   C
  !   |
     �
     �
     �
  E   �
       "      
   C  	   N     X  1  d     �  %   �  &   �  (         )  /   9  '   i  -   �     �     �     �  %   �  M     "   f  q   �     �     
       &   2  $   Y     ~     �     �     �     �     �     �  '     &   )     P     b     j  $   �     �  .   �  �   �  *   �  ^   �  �   .  8   �     �  G        K    f  .   i  �   �  	   )  	   3     =     C     I     R     X     l     �     �     �     �     �     �  
   �     �                         5     I  	   N     X  
   f     q     �  
   �     �  	   �     �     �  
   �  
   �  
   �     �       	               9     Z     b     n     s  	   {     �     �     �     �     �  *   �       	             !     &     5     L  
   R     ]     c  �  j     �  B     .   E     t     �     �  4   �     �  $   �       
   %     0    ?     \  :   v  )   �  )   �       *     9   ?  3   y     �     �     �  +   �  a     "   |  �   �  "   6  *   Y      �  F   �  E   �     2   "   R      u      �   &   �   	   �      �   &   �   ;   !     N!     e!     n!     �!  (   �!  -   �!    �!  1   #  �   K#  �   �#  \   �$  	   �$  \   �$  $   N%  D  s%  >   �&  �   �&  
   �'  	   �'     �'     �'  
   �'     �'  '   �'     (     )(     I(     `(     {(     �(  -   �(     �(     �(     �(     �(     �(  ;   �(     ;)     [)  
   `)     k)     �)  )   �)     �)     �)  "   �)     �)     �)     
*     *     *     +*  	   4*     >*     M*     U*  $   s*     �*  
   �*     �*     �*  	   �*     �*     �*     �*     �*     +  .   ++     Z+     h+     w+     �+  )   �+     �+     �+     �+     �+     �+        0   2                .              ,   d       l       y   B   k           U   #          "   '   a   m   +   D   =       -       )         x   A              j       c   r          
   \       b   <       p      f   5   /           O           `         G   w   Z   $   t   1   H   J          M   ;   7       R          P   i       ^      L   (   W      ]   6   s       !   F   T       &       8   Y   h   I   %       :          [       E           4   q                 N   V      _   ?   S   o                   3       X   *       Q   g   v                      n   e   	   u          C   >   @   9       K        (custom) %1 couldn't be executed successfully. error message:
 %2 %1 not found. Is grub2 installed? (new Entries of %1) (new Entries) (script code) AND: your modifications are still unsaved, update will save them too! A_ppearance Add a script to your configuration Add script BURG Mode BURG found! Before you can edit your grub configuration we have to
mount the required partitions.

This assistant will help you doing this.


Please ensure the target system is based on the same cpu architecture
as the actually running one.
If not, you will get an error message when trying to load the configuration. Configuration has been saved Couldn't mount the selected partition Couldn't umount the selected partition Customize the bootloader (GRUB2 or BURG) Default title:  Do you want to configure BURG instead of grub2? Do you want to save your modifications? Do you want to select another root partition? Edit grub preferences Entry Entry %1 (by position) Error while installing the bootloader Grub Customizer is a graphical interface to configure the grub2/burg settings Grub Customizer: Partition chooser Install the bootloader to MBR and put some
files to the bootloaders data directory
(if they don't already exist). Install to MBR Mount failed! Mount selected Filesystem Move down the selected entry or script Move up the selected entry or script Name the Entry No Bootloader found No script found Partition Chooser Please type a device string! Preview: Proxy binary not found! Remove a script from your configuration Save configuration and generate a new  Script to insert: Seconds Select _partition … Select and mount your root partition Select required submountpoints The bootloader has been installed successfully The generated configuration didn't equal to the saved configuration on startup. So what you see now may not be what you see when you restart your pc. To fix this, click update! The saved configuration is not up to date! These are the mountpoints of your fstab file.
Please select every grub/boot related partition. This option doesn't work when the "os-prober" script finds other operating systems. Disable "%1" if you don't need to boot other operating systems. This seems not to be a root file system (no fstab found) Timeout To get the colors above working,
you have to select a background image! Unmount mounted Filesystem You selected the option for choosing another partition.
Please note: The os-prober may not find your actually running system.
So run Grub Customizer on the target system
again and save the configuration (or run update-grub/update-burg)
to get the entry back! You started Grub Customizer using the live CD. You will see all entries (uncustomized) when you run grub. This error accurs (in most cases), when you didn't install grub gustomizer currectly. _Advanced _Device:  _Edit _File _General _Help _Install to MBR … _Quit without saving _Quit without update _Save & Quit _Update & Quit _View _use another partition:  add this entry to a new submenu background background image black blue brown cannot move this entry custom resolution:  cyan dark-gray default entry font color generate recovery entries green highlight: installing the bootloader… is active kernel parameters label light-blue light-cyan light-gray light-green light-magenta light-red loading configuration… look for other operating systems magenta menu colors name normal: partition pre_defined:  previously _booted entry red reload configuration remove background remove this entry from the current submenu settings show menu transparent type umount failed! updating configuration value visibility white yellow Project-Id-Version: 2.3
Report-Msgid-Bugs-To: 
POT-Creation-Date: 2011-12-23 17:05+0100
PO-Revision-Date: 2011-01-22 15:00+0000
Last-Translator: Daniel Richter <danielrichter2007@web.de>
Language-Team: German
Language: 
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
X-Launchpad-Export-Date: 2011-03-26 14:24+0000
X-Generator: Launchpad (build 12661)
  (angepasst) %1 konnte nicht erfolgreich ausgeführt werden. Fehlermeldung:
 %2 %1 nicht gefinden. Ist Grub2/BURG installiert? (neue Einträge von %1) (neue Einträge) (Script-Code) UND: Es gibt noch einige ungespeicherte Änderungen! _Darstellung Script zur Konfiguration hinzufügen Script hinzufügen BURG-Modus BURG gefunden! Bevor Sie Ihre Grub-Konfiguration bearbeiten können, müssen
noch einige Partitionen eingebunden werden.

Dieser Assistent wird Sie dabei unterstützen.


Beachten Sie, dass nur Systeme bearbeitet werden können,
welche die gleiche CPU-Architektur benutzen, wie das aktuell
Laufende. Konfiguration gespeichert Die ausgewählte Partition konnte nicht eingebunden werden Partition konnte nicht ausgehängt werden Den Bootloader anpassen (GRUB2 oder BURG) Vorgabetitel:  Wollen Sie BURG statt Grub2 konfigurieren? Sollen alle vorgenommenen Änderungen gespeichert werden? Möchten Sie eine andere Root-Partition auswählen? Grub-Einstellungen bearbeiten Eintrag Eintrag %1 (nach Position) Installation des Bootloaders fehlgeschlagen Grub Customizer ist eine grafische Benutzeroberfläche zur Anpassung der grub2/burg-Einstellungen Grub Customizer: Partitionsauswahl Dieser Dialog wird den Bootloader in den Master Boot Record
übertragen. Ggf. werden einige Dateien im Daten-Verzeichnis
des Bootloaders neu angelegt. Bootloader in den MBR installieren Partition konnte nicht eingebunden werden! Gewähltes Dateisystem einbinden Aktuellen Eintrag / aktuelles Script eine Position nach unten schieben Aktuellen Eintrag / aktuelles Script eine Position nach oben schieben Geben Sie eine Bezeichnung ein! Es wurde kein Bootloader gefunden! Keine Skripte gefunden Partitionsauswahl Bitte geben Sie einen Geräte-Pfad ein Vorschau: Proxy-Datei nicht gefunden! Script aus der Konfiguration entfernen Konfiguration speichern und folgende Datei neu generieren:  Einzufügendes Script: Sekunden _Partition auswählen … Wählen Sie Ihre Root-Partition Wählen Sie alle notwendigen Mountpoints Der Bootloader wurde erfolgreich installiert! Die generierte Konfiguration entsprach nicht der gespeicherten Konfiguration, als Sie dieses Programm gestartet haben. Dies kann dazu führen, dass die Anzeige im Bootloader nicht der Anzeige dieses Programms entspricht. Um dieses Problem zu beheben, klicken Sie auf Aktualisieren! Die gespeicherte Konfiguration ist nicht aktuell! Diese Mountpoints befinden sich in Ihrer fstab.
Bitte wählen Sie alle Partitionen, die zum
Generieren der Grub-Konfiguration notwendig sind. Diese option funktioniert nur, wenn der "os-prober" keine anderen Betriebssystemefindet. Deaktivieren Sie "%1" wenn Sie generell keine anderen Betriebssysteme starten möchten. Bei dieser Partition scheint es sich nicht um ein Wurzeldateisystem zu handeln (fstab fehlt) Wartezeit Damit die ausgewählen Farben verwendet werden,
muss ein Hintergrundbild ausgewählt werden! Eingebundenes Dateisystem aushängen Sie haben den Dialog zur Auswahl einer anderen Partition
manuell gestartet. Bitte beachten Sie: Möglicherweise erkennt
der os-prober das aktuell laufende System nicht.
Sollte dies der Fall sein, führen Sie den Grub Customizer
zusätzlich im Ziel-System aus und klicken Sie auf Speichern,
um den Eintrag wiederherzustellen! Sie haben den Grub Customizer von einer Live-CD aus gestartet. Wenn Sie Grub das nächste mal starten, sehen Sie alle Einträge (unverändert).Dieser Fehler tritt in der Regel dann auf wenn der Grub Customizer nicht korrekt installiert wurde. _Erweitert _Gerät:  _Bearbeiten _Datei _Allgemein _Hilfe Bootloader in den MBR _installieren … _Beenden, ohne zu speichern _Beenden, ohne zu aktualisieren _Speichern und Beenden _Aktualisieren und Beenden _Ansicht _Andere Partition verwenden:  Diesen Eintrag in ein neues Submenü schieben Hintergrund Hintergrundbild Schwarz Blau Braun Dieses Element kann nicht wie gewünscht verschoben werden. benutzerdefinierte Auflösung:  Cyan Dunkelgrau Vorausgewählter Eintrag Schriftfarbe "Wiederherstellungs-Einträge" generieren Grün Hervorgehoben: der Bootloader wird installiert… Aktiv Kernel-Parameter Bezeichnung Hellblau Helles Cyan Hellgrau Hellgrün Helles Magenta Hellrot Konfiguration wird geladen… Nach anderen Betriebssystemen suchen Magenta Menufarben Bezeichnung Normal: Partition Voreingestellt:  _Vorher gestarteter Eintrag Rot Konfiguration erneut laden Hintergrund entfernen Diesen Eintrag aus dem Submenü herausschieben Einstellungen Menü anzeigen Transparent Typ Partition konnte nicht ausgehängt werden Konfiguration wird aktualisiert Wert Anzeige Weiß Gelb 