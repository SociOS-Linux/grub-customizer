��    s      �  �   L      �	  	   �	  >   �	     

     *
  E   8
     ~
  "   �
  
   �
  	   �
     �
  1  �
        %     &   C     j  /   z  '   �  -   �                  %   3  M   Y  "   �  q   �     <     K     Y  &   s  $   �     �     �     �     �          !     *  '   B  &   j     �     �     �  $   �     �  .     �   4  *   �  ^     �   o  8        <  G   D     �    �  .   �  �   �  	   j  	   t     ~     �     �     �     �     �     �     �     �     �     �  
             .     4     9     ?     S  	   X     b  
   p     {     �  
   �     �  	   �     �     �  
   �  
   �  
   �            	         *      C     d     l     x     }  	   �     �     �     �     �     �     �  	   �     �                     +  
   1     <     B  �  I  0   �  j     :   ~  "   �  w   �     T  F   j  !   �     �     �  �  �  +   �  J   �  H   %  *   n  g   �  A     g   C  (   �     �  0   �  W     j   q  *   �  �         �   +   !  G   4!  L   |!  L   �!     "  7   3"  $   k"     �"  9   �"  0   �"  9   #  F   O#  I   �#  "   �#     $      $  H   3$  b   |$  R   �$  �  2%  ?   �&  �   �&    �'  h   �(     &)  �   B)  I   �)    $*  B   ),  +  l,     �-     �-     �-  	   �-     �-     �-      .  '   .  '   E.  !   m.     �.     �.  5   �.     �.  %   �.     /  
   '/     2/  -   G/     u/     �/  $   �/     �/  P   �/     00     ?0  *   S0     ~0     �0  
   �0     �0     �0  !   �0     1     ,1     L1  ,   h1  =   �1     �1     �1     �1     2     2      (2  9   I2     �2  3   �2     �2     �2     �2     3     3  )   #3  -   M3     {3     �3  
   �3     �3     !   O   N   	   f       C      "          1   
       s              (   D      G   )   m       @   g   Q       <      ;           a   9   J      $   >   L          Z       8          -      k       K      j   4       ?   ^       I       &          X   .          %   5   F   U   6                 V   i              B           Y   P   H   S           2      _   c   0   o          =                  p   W      h      d   M       b   ]           #           7                R      :   e   `   /          ,      *   r   q   +      n   T       3       E       l       A      [       '       \     (custom)  couldn't be executed successfully. You must run this as root!  not found. Is grub2 installed? (new Entries) AND: your modifications are still unsaved, update will save them too! A_ppearance Add a script to your configuration Add script BURG Mode BURG found! Before you can edit your grub configuration we have to
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
to get the entry back! You started Grub Customizer using the live CD. You will see all entries (uncustomized) when you run grub. This error accurs (in most cases), when you didn't install grub gustomizer currectly. _Advanced _Device:  _Edit _File _General _Help _Install to MBR … _Quit without saving _Quit without update _Save & Quit _Update & Quit _View _use another partition:  background background image black blue brown custom resolution:  cyan dark-gray default entry font color generate recovery entries green highlight: installing the bootloader… is active kernel parameters label light-blue light-cyan light-gray light-green light-magenta light-red loading configuration… look for other operating systems magenta menu colors name normal: partition pre_defined:  previously _booted entry red reload configuration remove background settings show menu transparent type umount failed! updating configuration value visibility white yellow Project-Id-Version: grub-customizer
Report-Msgid-Bugs-To: FULL NAME <EMAIL@ADDRESS>
POT-Creation-Date: 2011-09-14 00:10+0200
PO-Revision-Date: 2011-09-14 19:50+0000
Last-Translator: jmb_kz <Unknown>
Language-Team: Russian <ru@li.org>
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
X-Launchpad-Export-Date: 2011-10-22 22:16+0000
X-Generator: Launchpad (build 14170)
  (добавлено пользователем)  не может быть выполнено успешно. Вы должны иметь права root!  не найден. GRUB2 точно установлен? (новые пункты меню) И: внесённые вами изменения не сохранены, обновление сохранит их! Внешний вид Добавить этот сценарий в конфигурацию Добавить сценарий Режим BURG Обнаружен BURG! Перед тем как редактировать конфигурацию grub,
необходимо подключить соответствующие разделы.

Помощник поможет вам выполнить это действие.


Please ensure the target system is based on the same cpu architecture
as the actually running one.
If not, you will get an error message when trying to load the configuration. Конфигурация сохранена Не возможно подключить выбранный раздел Не возможно отключить выбранный раздел Название по умолчанию:  Вы действительно хотите сконфигурировать BURG, вместо GRUB2? Вы хотите сохранить ваши изменения? Вы действительно хотите выбрать другой корневой раздел? Изменить настройки GRUB Пункт меню Пункт меню %1 (по положению) Ошибка в процессе установки программы загрузки Grub Customizer - это графический интерфейс для настройки grub2 и burg Grub Customizer: Выбор раздела Установите программу загрузки в MBR и разместите 
несколько файлов в папку данных программы загрузки
(если они не существуют). Установить в MBR Ошибка при подключении! Подключить выбранные файловые системы Переместить ниже, пункт меню или сценарий Переместить выше, пункт меню или сценарий Имя пункта меню Программа загрузки не найдена Сценарии не найдены Выбор раздела Пожалуйста укажите устройство! Предварительный просмотр: Двоичный файл прокси не найден! Удалить этот сценарий из конфигурации Сохранить и создать новую конфигурацию  Добавить сценарий: Секунды Выбрать _раздел ... Выберите и смонтируйте корневой раздел Выберите запрашиваемые зависимые точки монтирования Программа загрузки была успешно установлена Созданная конфигурация не соответствует сохранённой конфигурации при загрузке. То, что вы видите сейчас, может не соответствовать  после выполнения перезагрузки компьютера. Нажмите обновить, для исправления! Сохраненая конфигурация устарела! Это точки монтирования описанные в вашем файле fstab
Пожалуйста, выберите каждый загрузочный раздел. Эта возможность не работает, если сценарий найдёт другие операционные системы. Отключите "%1", если вы не хотите загружать другие операционные системы. Вероятно, это не корневая файловая система (fstab не найден) Время ожидания Для использования цветов выбранных выше,
вам необходимо выбрать фоновую картинку! Отключить подключеные файловые системы Вы выбрали возможность назначения другого раздела.
Обратите внимание: программа может не найти работающую в данный момент систему.
Запускайте Grub Customizer на выбранной рабочей системе
снова и сохраните конфигурацию (или выполните update-grub/update-burg)
для восстановления пункта меню обратно! Вы запустили Grub Customizer используя LiveCD. Вы будете видеть все пункты меню (не добавленные пользователем) при загрузке grub. Эта ошибка появляется (в большинстве случаев), при неправильной установке grub gustomizer. _Дополнительно _Устройство:  _Правка _Файл _Общие _Справка _Установить в MBR Вы_ход без обновления В_ыход без обновления _Сохранить и выйти О_бновить и выйти _Вид _использовать другой раздел:  фон фоновое изображение чёрный синий коричневый собственное разрешение:  голубой тёмно-коричневый запись по умолчанию цвет шрифта создать пункты меню режимов восстановления зелёный Выделение: установка загрузчика... Активность параметры ядра метка светло-синий светло-голубой светло-коричневый светло-зелёный светло-пурпурный светло-красный Загрузка конфигурации... поиск других операционных систем пурпурный цвета меню Название обычный: раздел предопределённо:  предыдущая за_груженная запись красный Перезагрузить конфигурацию убрать фон Настройки показать меню прозрачный тип ошибка при отключении! обновление конфигурации Значение видимый белый жёлтый 