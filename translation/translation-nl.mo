��    u      �  �   l      �	  	   �	  >   �	     *
     J
     ^
  E   l
     �
  "   �
  
   �
  	   �
     �
  1       4  %   Q  &   w     �  /   �  '   �  -        4     J     P  %   g  M   �  "   �  q   �     p          �  &   �  $   �     �               &     8     U     ^  '   v  &   �     �     �     �  $   �       .   9  �   h  *     ^   D  �   �  8   7     p  G   x     �    �  .   �  �     	   �  	   �     �     �     �     �     �     �     �               '     -  
   F     Q     b     h     m     s     �     �  	   �     �  
   �     �     �  
   �     �  	             *  
   0  
   ;  
   F     Q     ]  	   k     u      �     �     �     �     �  	   �     �     �                    ,  	   5     ?     K     P     _     v  
   |     �     �  �  �     3  C   @  (   �     �     �  h   �  	   A  (   K     t  
   �     �  <  �     �  .   �  -   &     T  -   e     �  +   �     �     �     �  ,     b   D  $   �  ~   �     K     ^  (   s  #   �  #   �     �     �          ,     @  
   V     a  *   }  '   �     �     �     �  (   
   #   3   +   W   �   �   .   i!  ^   �!  �   �!  A   �"     �"  _   �"  '   @#     h#  /   �$  �   �$     \%     i%  	   u%     %  	   �%     �%     �%     �%     �%     �%     &     &     !&     @&     L&     b&     h&     n&      t&     �&     �&     �&     �&     �&     �&     �&     �&  &   	'  	   0'     :'     L'  
   R'  
   ]'  
   h'  
   s'     ~'  	   �'     �'  #   �'     �'     �'     �'     �'     �'     (     (     -(     2(     H(     `(  
   m(     x(     �(     �(     �(     �(     �(     �(     �(     "   Q   P   
   h       D   	   #          2          u          !   )   E      H   *   o       A   i   S       =      <   M       c   :   K      %   ?   N          \       9          .      m       L      4   5       @   `       J       '          Z   /          &   6   G   W   7                  X   k              C           [   R   I   U           3      a   e   1   q          >                  r   Y      j      f   O       d   _           $           8                T      ;   g   b   0          -      +   t   s   ,      p   V              F       n   l   B      ]       (       ^     (custom)  couldn't be executed successfully. You must run this as root!  not found. Is grub2 installed? (new Entries of %1) (new Entries) AND: your modifications are still unsaved, update will save them too! A_ppearance Add a script to your configuration Add script BURG Mode BURG found! Before you can edit your grub configuration we have to
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
to get the entry back! You started Grub Customizer using the live CD. You will see all entries (uncustomized) when you run grub. This error accurs (in most cases), when you didn't install grub gustomizer currectly. _Advanced _Device:  _Edit _File _General _Help _Install to MBR … _Quit without saving _Quit without update _Save & Quit _Update & Quit _View _use another partition:  background background image black blue brown cannot move this entry custom resolution:  cyan dark-gray default entry font color generate recovery entries green highlight: installing the bootloader… is active kernel parameters label light-blue light-cyan light-gray light-green light-magenta light-red loading configuration… look for other operating systems magenta menu colors name normal: partition pre_defined:  previously _booted entry red reload configuration remove background settings show menu transparent type umount failed! updating configuration value visibility white yellow Project-Id-Version: grub-customizer
Report-Msgid-Bugs-To: FULL NAME <EMAIL@ADDRESS>
POT-Creation-Date: 2011-09-14 00:10+0200
PO-Revision-Date: 2011-09-17 07:38+0000
Last-Translator: rob <linuxned@gmail.com>
Language-Team: Dutch <nl@li.org>
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
X-Launchpad-Export-Date: 2011-10-22 22:16+0000
X-Generator: Launchpad (build 14170)
  (aangepast)  kon niet uitgevoerd worden. Kan alleen als root uitgevoerd worden!  niet gevonden. Is grub2 geïnstalleerd? (nieuwe invoeren van %1) (nieuwe invoeren) EN: uw wijzigingen zijn niet opgeslagen, door bij te werken zullen de wijzigingen ook opgeslagen worden! Wee_rgave Een script toevoegen aan uw configuratie Script toevoegen BURG-modus BURG gevonden! Voordat u uw grub-configuratie kunt bewerken, moeten we
de vereiste partities aankoppelen.

Deze assistent zal u daarbij helpen


Controleer of het doelsysteem dezelfde cpu-architectuur heeft
als het momenteel actieve systeem.
Indien dit niet zo is, zal u een foutmelding ontvangen bij het laden van de configuratie. Configuratie is opgeslagen Kon de geselecteerde partitie niet aankoppelen Kon de geselecteerde partitie niet afkoppelen Standaardtitel:  Wilt u BURG configureren in plaats van grub2? Wilt u uw wijzigingen opslaan? Wilt u een andere root-partitie selecteren? Grub-voorkeuren bewerken Invoer Invoer %1 (naar positie) Fout bij het installeren van de opstartlader Grub Customizer is een grafische gebruikersinterface om de grub2/burg-instellingen te configureren Grub Customizer: Partitie selecteren Installeer de opstartlader in MBR en voeg enkele
bestanden toe aan de opstartladerdatamap
(als deze niet reeds aanwezig zijn). Installeren in MBR Aankoppelen mislukt! Geselecteerd bestandssysteem aankoppelen Invoer of script omlaag verplaatsen Invoer of script omhoog verplaatsen Geef de invoer een naam Geen opstartlader gevonden Geen script gevonden Partitie selecteren Voer een apparaat in! Voorbeeld: Proxy binary niet gevonden! Een script verwijderen uit uw configuratie Sla configuratie op en genereer nieuwe  Script om in te voegen: Seconden _Partitie selecteren … Selecteer en koppel uw root-partitie aan Selecteer vereiste sub-koppelpunten De opstartlader is succesvol geïnstalleerd De gegenereerde configuratie is niet gelijk aan de opgeslagen configuratie bij het opstarten. Dus wat u nu ziet hoeft niet noodzakelijkerwijs hetzelfde te zijn wanneer u uw pc herstart. Om dit te herstellen, klikt u op bijwerken! De opgeslagen configuratie is niet up-to-date! Dit zijn de koppelpunten van uw fstab-bestand.
Selecteer elke gerelateerde grub/boot-partitie. Deze optie werkt niet wanneer het script "os-prober" andere besturingssytemen vindt. Schakel "%1" uit wanneer u geen andere besturingssytemen gaat gebruiken. Dit lijkt geen root-bestandssysteem te zijn (geen fstab gevonden) Time-out Om de kleuren hierboven te kunnen gebruiken,
moet u eerst een achtergrondafbeelding selecteren! Aangekoppeld bestandssysteem afkoppelen U heeft de optie geselecteerd om een andere partitie te selecteren.
Let op: Mogelijk vindt os-prober het momenteel actieve systeem niet.
Voer Grub Customizer opnieuw uit op het doelsysteem
en sla de configuratie op (of voer update-grub/update-burg uit)
om de invoer weer terug te krijgen! U heeft Grub Customizer gestart met de live-cd. Wanneer u grub start zult u alle invoeren (onaangepast) zien. Deze fout doet zich (in de meeste gevallen) voor, wanneer grub customizer onjuist is geïnstalleerd. Ge_avanceerd A_pparaat:  Be_werken _Bestand Al_gemeen _Hulp _Installeren in MBR … A_fsluiten zonder op te slaan A_fsluiten zonder bij te werken Op_slaan & afsluiten _Bijwerken & afsluiten Beel_d gebr_uik een andere partitie:  achtergrond achtergrondafbeelding zwart blauw bruin kan deze invoer niet verplaatsen aangepaste resolutie:  cyaan donkergrijs standaardinvoer letterkleur herstelinvoeren genereren groen geaccentueerd: De opstartader wordt geïnstalleerd… is actief kernel-parameters label lichtblauw lichtcyaan lichtgrijs lichtgroen lichtmagenta lichtrood configuratie wordt geladen… zoek naar andere besturingssystemen magenta menukleuren naam normaal: partitie vooringestel_d:  vorige opge_starte invoer rood configuratie herladen achtergrond verwijderen instellingen menu tonen doorzichtig type afkoppelen mislukt! configuratie bijwerken waarde zichtbaarheid wit geel 