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
   q     |     �  �  �     O  9   `     �     �     �     �  O   �  
   C  -   N     |     �     �    �     �  $   �  #   
     .  ;   H  +   �  6   �  "   �     
       /   .  J   ^  &   �  �   �     \     v  %   �  4   �  6   �           8      X      k   (   �      �      �   +   �   0   �      0!     K!     P!  6   j!  +   �!  )   �!  �   �!     �"  t   #  �   x#  C   $     Z$  V   h$  $   �$  	  �$  ?   �%  ~   .&  
   �&     �&     �&     �&  	   �&     �&  	   �&     �&     '     &'     <'     Q'     g'     p'  )   �'     �'     �'     �'     �'     �'     �'     �'     (     (     '(     =(     L(     i(     p(      y(     �(     �(  
   �(     �(     �(     �(     �(     )      )     2)  #   N)  
   r)     })     �)  
   �)     �)     �)  !   �)     �)     �)     *     *  +   &*  
   R*  
   ]*     h*  	   u*     *     �*     �*  	   �*  	   �*     �*     �*         N   D   3   h       =   0   g   Y   C              .   Z                      l   w                      S      ,       y       :   5   U   ;   @   4   `       (   2   )       x         8      e       j   /           f   9      >   K             F       n   E   V                 	   c              m      1                   Q           O   [   %       o      u   +   d   t   
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
PO-Revision-Date: 2012-06-20 04:48+0000
Last-Translator: Algimantas Margevičius <Margevicius.Algimantas@gmail.com>
Language-Team: Lithuanian <lt@li.org>
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
X-Launchpad-Export-Date: 2012-08-30 21:28+0000
X-Generator: Launchpad (build 15877)
  (pasirinktinis) Sėkmingai įvykdyti %1 nepavyko. Klaidos pranešimas:
%2 %1 nerasta. Grub2 įdiegta? (nauji įrašai iš %1) (nauji įrašai) (scenarijaus kodas) TAIPOGI: jūsų pakeitimai vis dar nėra įrašyti, atnaujinimas tai padarytų! _Išvaizda Pridėkite scenarijų į savo konfigūraciją Pridėti scenarijų BURG režimas Aptiktas BURG įkėliklis! Norint redaguoti grub konfigūraciją reikia prijungti
reikiamus skirsnius.

Šis vedlys padės tai padaryti.


Įsitikinkite jog paskirties sistema naudoja tokią pačią cpu architektūrą
kaip ir veikianti.
Jei skiriasi, įkeliant konfigūraciją gausite klaidos pranešimą. Nustatymai įrašyti Nepavyko prijungti parinkto skirsnio Nepavyko atjungti parinkto skirsnio Numatytasis pavadinimas:  Ar norėtumėte konfigūruoti įkėliklį BURG, o ne GRUB2? Ar norite įrašyti dabartinius pakeitimus? Ar norėtumėte pasirinkti kitą pagrindinį skirsnį? Keisti įkėliklio GRUB nustatymus Įrašas Įrašas %1 (pagal vietą) Įvyko klaida įdiegiant OS paleidimo programą Grub Customizer - tai grafinis įrankis skirtas grub2/burg konfigūravimui Grub Customizer: Skirsnio pasirinkimas Įdiegti OS paleidimo programą į laikmenos MBR sektorių
ir, jei reikia, sudėti reikalingus failus į OS paleidiklio 
duomenų aplanką. Įdiegti į MBR sektorių Prijungimas nepavyko! Prijungti pasirinktą failų sistemą Perkelkite žemyn pasirinktą įrašą ar scenarijų Perkelkite aukštyn pasirinktą įrašą ar scenarijų Pavadinkite šį įrašą Neaptiktas Linux OS įkėliklis Scenarijų nerasta Skirsnių pasirinkimas Nurodykite įrenginio failo pavadinimą! Peržiūra: Tarpinis serveris nerastas! Pašalinkite scenarijų iš konfigūracijos Įrašyti konfigūraciją ir sugeneruoti naują  Scenarijus kurį įterpti: sek. Pasirinkite _skirsnį ... Parinkite bei prijunkite pagrindinį sistemos skirsnį Pasirinkite reikiamus subprijungimo taškus OS paleidimo programa įdiegta sėkmingai Sugeneruoti nustatymai neatitinka paleidimo metu įrašytos konfigūracijos. Tai, ką matote dabar gali neatitikti realios situacijos, kurią matysite paleidę kompiuterį iš naujo. Norėdami tai pataisyti - spauskite „Atnaujinti“! Išsaugoti nustatymai pasenę! Tai yra jūsų „fstab“ failo prijungimo taškai.
Pasirinkite visus grub/boot įrašus kurie susiję su skirsniu. Ši parinktis neveikia kai „os-prober“ scenarijus randa kitų operacinių sistemų. Išjunkite „%1“ jei nenorite įkrauti kitų operacinių sistemų. Tai tikriausiai nėra pagrindinė failų sistema (neaptiktas fstab) Laiko limitas Norint jog aukščiau esančios spalvos veiktų,
turite pasirinkti fono paveikslėlį! Atjungti pasirinktą failų sistemą Pasirinkote parinkty kito skirsnio pasirinkimui.
Pastaba: os-prober gali nerasti jūsų sistemos.
Taigi norėdami vėl matyti įrašą dar kartą paleiskite 
Grub Customizer paskirties sistemoje ir išsaugokite 
nustatymus (arba įvykdykite update-grub/update-burg) Paleidote Grub Customizer iš demonstracinės (live) laikmenos. Kai paleisite grub matysite visus įrašus(nederintus). Ši klaida dažniausiai įvyksta kai gustomizer įdiegtas neteisingai. _Papildoma Į_renginys:  _Keisti _Failas Šri_ftas _Bendra _Žinynas Į_diegti į MBR sektorių ... _Baigti neišsaugant _Baigti neatnaujinant Į_rašyti ir baigti _Atnaujinti ir baigti _Rodymas _naudoti kitą skirsnį:  pridėti šį įrašą į naują submeniu fonas fono paveikslėlis juoda mėlyna ruda perkelti įrašo nepavyko pasirinktinė raiška:  žydra tamsiai pilka numatytasis elementas šrifto spalva generuoti atkūrimo įrašus žalia ryškus: įdiegiama OS paleidimo programa yra aktyvus branduolio parametrai antraštė šviesiai mėlyna šviesiai žydra šviesiai pilka šviesiai žalia šviesiai purpurinė šviesiai raudona įkeliama konfigūracija… ieškoti kitų operacinių sistemų purpurinė meniu spalvos pavadinimas įprastas: skirsnis _numatytasis:  _paskutinis pasirinktas elementas raudona Įkelti nustatymus iš naujo pašalinti foną pašalinti šriftą pašalinti įrašą iš dabartinio submeniu išsaugota nustatymai rodyti meniu permatoma tipas atjungimas nepavyko! nustatymai atnaujinami reikšmė matomumas balta geltona 