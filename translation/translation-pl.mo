��    u      �  �   l      �	  	   �	  "   �	  >   
     M
     m
  E   {
     �
  "   �
  
   �
  	   �
       1       C  %   `  &   �     �  /   �  '   �  -        C     Y     _  %   v  M   �  "   �  q             �     �  &   �  $   �               %     5     G     d     m  '   �  &   �     �     �     �  $        )  .   H  �   w  *   (  ^   S  �   �  8   F       G   �     �    �  .   �  �     	   �  	   �     �     �     �     �     �     �               '     6     <  
   U     `     q     w     |     �     �     �  	   �     �  
   �     �     �  
   �        	        '     9  
   ?  
   J  
   U     `     l  	   z     �      �     �     �     �     �  	   �     �     �               )     ;  	   D     N     Z     _     n     �  
   �     �     �  �  �  
   H  2   S  B   �  *   �     �  :        <     E     b  	   o     y    �     �  -   �  (   �     	  !        >     O     m     �     �  0   �  K   �        o   =     �     �  $   �  -     .   0     _  $   k     �     �     �  	   �     �       '        E     [     b  !   y  (   �  3   �  �   �  )   �   `   �   �   C!  6   �!     "  @   5"     v"  �   �"  0   k#  �   �#     9$     G$     V$     ^$     d$     m$     t$     �$     �$     �$     �$     �$     �$     %     	%     %  	   %     '%     0%     J%     d%     u%     �%     �%     �%     �%     �%  #   �%     �%     &     &      &     0&     <&     H&     V&     e&     t&  $   �&     �&     �&     �&  
   �&     �&     �&     �&     '     '  
   7'  
   B'     M'     Y'     k'      o'     �'  	   �'     �'     �'     �'     "   Q   P   
   h       D   	   #          2          u          !   )   E      H   *   o       A   i   S       =      <           c   :   K      %   ?   N          \       9          .      m       L      l   5       @   `       J       '          Z   /             6   G   W   7                  X   k          M   C           [   R   I   U           3      a   e   1   q          >                  r   Y      j      f   O       d   _           $       &   8                T      ;   g   b   0          -      +   t   s   ,      p   V       4       F       n       B      ]       (       ^     (custom)  command not found, cannot proceed  couldn't be executed successfully. You must run this as root!  not found. Is grub2 installed? (new Entries) AND: your modifications are still unsaved, update will save them too! A_ppearance Add a script to your configuration Add script BURG Mode BURG found! Before you can edit your grub configuration we have to
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
to get the entry back! You started Grub Customizer using the live CD. You will see all entries (uncustomized) when you run grub. This error accurs (in most cases), when you didn't install grub gustomizer currectly. _Advanced _Device:  _Edit _File _General _Help _Install to MBR … _Quit without saving _Quit without update _Save & Quit _Update & Quit _View _use another partition:  background background image black blue brown copy to grub directory custom resolution:  cyan dark-gray default entry font color generate recovery entries green highlight: installing the bootloader… is active kernel parameters label light-blue light-cyan light-gray light-green light-magenta light-red loading configuration… look for other operating systems magenta menu colors name normal: partition pre_defined:  previously _booted entry red reload configuration remove background settings show menu transparent type umount failed! updating configuration value visibility white yellow Project-Id-Version: grub-customizer
Report-Msgid-Bugs-To: FULL NAME <EMAIL@ADDRESS>
POT-Creation-Date: 2010-11-28 22:09+0100
PO-Revision-Date: 2011-01-22 15:00+0000
Last-Translator: Adam Czabara <kolofaza@o2.pl>
Language-Team: Polish <pl@li.org>
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
X-Launchpad-Export-Date: 2011-03-26 14:24+0000
X-Generator: Launchpad (build 12661)
  (dowolny)  polecenie nieodnalezione, nie można kontynuować  nie udało się wykonać pomyślnie. Musisz uruchomić jako root!  nie znaleziono. Grub2 jest zainstalowany? (nowe wpisy) Modyfikacje nie są zapisane, aktualizacja zapisze zmiany! W_ygląd Dodaj skrypt do konfiguracji Dodaj skrypt Tryb BURG Znaleziono BURG! Przed edycją konfiguracji grub należy
zamontować wymagane partycje.

Asystent pomoże Ci to zrobić.

Upewnij się że system docelowy ma tą samą architekturę procesora
jak aktualnie używany
Jeżeli nie, zostanie wyświetlony błąd podczas ładowania konfiguracji. Zapisano konfigurację Nie udało się zamontować wybranej partycji Nie można odmontować wybranej partycji Domyślny tytuł:  Skonfigurować BURG zamiast grub2 Zapisać zmiany? Wybrać inną partycję root? Edycja preferencji grub Wpis Wpis %1 (wg pozycji) Błąd w czasie instalacji programu rozruchowego Grub Customizer to graficzny interfejs do konfiguracji ustawień grub2/burg Grub Customizer: wybór partycji Instaluje program rozruchowy i umieszcza
pliki w katalogu danych programu rozruchowego
(jeżeli nie istnieją). Instalacja w MBR Montowanie nie powiodło się! Montowanie wybranego systemu plików Przenieś w dół wybraną pozycji lub skrypt Przenieś w górę wybraną pozycji lub skrypt Nazwij wpis Nie znaleziono programu rozruchowego Nie znaleziono skryptu Wybór partycji Prosze wpisać ciąg urzadzenia Podgląd: Nie znaleziono binarnego Proxy! Usuń skrypt z konfiguracji Zapisz konfigurację i wygeneruj nową  Skrypt do wstawienia: Sekund Wybierz _partycję ... Wybierz i zamontuj partycję root Wybór wymaganych punktów podmontowania Program rozruchowy został zainstalowany pomyślnie Wygenerowana konfiguracja jest inna niż konfiguracja zapisana podczas uruchomienia. Widniejąca konfiguracja może być inna podczas restartu komputera. W celu naprawy kliknij aktualizację! Zapisana konfiguracja jest przestarzała! To są punkty montowania pliku fstab.
Proszę wybrać każdą partycję powiązaną z grub/boot. Ta opcja nie działa, gdy skrypt "os-prober" znajdzie inne systemy operacyjne. Wyłącz "%1", jeżeli nie potrzebujesz uruchamiać innych systemów operacyjnych. To nie jest system plików root (nie znaleziono fstab) Przekroczony limit czasu Powyższe kolory działają,
jeśli zostanie wybrany obraz tła! Odmontowanie systemu plików Wybrano inną partycję.
Można nie odnaleźć aktualnie używanego systemu.
Uruchom Grub Customizer na docelowym systemie
ponownie i zapisz konfigurację (lub uruchom update-grub/update-burg)
do odzyskania wpisów! Uruchomiono Grub Customize przy użyciu Live CD. Zobaczysz wszystkie wpisy (niedostosowane), gdy uruchomisz grub. Ten błąd występuje (najczęściej), jeżeli nie zainstalowano poprawnie grub customizer. Z_aawansowane _Urządzenie:  _Edycja _Plik _Ogólne _Pomoc _Instalacja w MBR _Zamknięcie bez zapisywania _Zamknij bez aktualizacji _Zapisz i zamknij _Aktualizuj i zamknij _Widok _Użyj innej partycji:  tło obrazek tła czarny niebieski brązowy skopij do katalogu grub'a własna rozdzielczość:  niebieskozielony ciemno-szary domyślny wpis kolor czcionki utwórz wpisy odzyskiwania zielony podświetlenie: Instalacja programu rozruchowego... jest aktywne ustawienia kernela etykieta jasno-niebieski jasny cyjan jasno-szary jasno-zielony jasny-czerwony jasno-czerwony ładowanie konfiguracji... szukaj innych systemów operacyjnych magenta kolory menu nazwa normalnie: partycja wstępnie zdefiniowane:  poprzednio bootowany wpis czerwony przeładuj konfigurację usuń tło ustawienia pokaż menu przezroczystość typ Odmontowanie nie powiodło się! Aktualizacja konfiguracji wartość widoczność biały żółty 