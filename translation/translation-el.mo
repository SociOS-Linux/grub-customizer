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
          !     *  '   B  &   j     �     �  $   �     �  .   �  �   ,  *   �  ^     8   g     �    �  .   �  �   �  	   ~     �     �     �     �     �     �     �     �     �     �       	   0     :     @     Y  	   ^  <   h     �     �     �  �  �     �  J   �  z   �  E   f  #   �  :   �          #  �  =  0   �  Z   %  `   �  +   �  P     T   ^  U   �  9   	     C  E   X  �   �  M   A  �   �  !   c  (   �  V   �  Z     Z   `  ,   �      �     	  )   &  g   P     �  #   �  ?   �  Q   8  $   �  (   �  ^   �  V   7  9   �  �  �  R   l!  �   �!  y   �"  \   #     _#  g   �%  R  �%     ;'     M'     e'     s'  &   �'  -   �'  +   �'  )   (  '   .(     V(  -   f(  +   �(     �(     �(  (   �(  
   )     )  �   .)  
   �)  .   �)  )   �)     E          (      )                 '   <                    
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
PO-Revision-Date: 2010-10-25 14:28+0000
Last-Translator: Daniel Richter <danielrichter2007@web.de>
Language-Team: Greek <el@li.org>
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
X-Launchpad-Export-Date: 2010-11-16 19:12+0000
X-Generator: Launchpad (build Unknown)
  (προσαρμοσμένη)  δεν βρέθηκε η εντολή, αδύνατη η συνέχεια  δεν ήταν δυνατή η επιτυχής εκτέλεση. Πρέπει να το εκτελέσετε ως root!  δεν βρέθηκε. Έχει εγκατασταθεί το grub2; (νέες καταχωρήσεις) Προσθήκη script στη διαμόρφωσή σας Προσθήκη script Βρέθηκε το BURG! Πριν μπορέσετε να επεξεργασθείτε την διαμόρφωση του grub σας πρέπει να
προσαρτήσετε τις απαραίτητες κατατμήσεις.

Αυτός ο βοηθός θα σας βοηθήσει σε αυτό.


Παρακαλώ βεβαιωθείτε ότι το σύστημα στόχος βασίζεται στην ίδια αρχιτεκτονική επεξεργαστή
με αυτό που πραγματικά εκτελείται αυτή τη στιγμή.
Αν όχι, θα εμφανιστεί ένα μήνυμα λάθους κατά την προσπάθεια φόρτωσης της διαμόρφωσης. Η διαμόρφωση αποθηκεύτηκε Αποτυχία προσάρτησης της επιλεγμένης κατάτμησης Αποτυχία αποπροσάρτησης της επιλεγμένης κατάτμησης Προεπιλεγμένος τίτλος:  Θέλετε να διαμορφώσετε το BURG αντί για το grub2; Θέλετε να αποθηκεύσετε τις τροποποιήσεις σας; Θέλετε να επιλέξετε άλλη ριζική (root) κατάτμηση; Επεξεργασία προτιμήσεων του grub Καταχώρηση Σφάλμα κατά την εγκατάσταση του bootloader Ο Διαμορφωτής του Grub είναι μια γραφική διεπαφή για τη διαμόρφωση των ρυθμίσεων του grub2/burg Διαμορφωτής του Grub: Επιλογέας κατατμήσεων Εγκατάσταση του bootloader στο MBR και τοποθέτηση μερικών
αρχείων στον κατάλογο δεδομένων των bootloader
(αν δεν υπάρχουν ήδη). Εγκατάσταση στο MBR Αποτυχία προσάρτησης! Προσάρτηση του επιλεγμένου συστήματος αρχείων Μετακινήστε κάτω την επιλεγμένη καταχώρηση ή script Μετακινήστε πάνω την επιλεγμένη καταχώρηση ή script Ονομάστε την καταχώρηση Δεν βρέθηκε Bootloader Δεν βρέθηκε script Επιλογέας κατατμήσεων Παρακαλούμε πληκτρολογήστε ένα αλφαριθμητικό συσκευής! Προεπισκόπηση: Δεν βρέθηκε proxy binary! Αφαίρεση script από τη διαμόρφωσή σας Αποθήκευση διαμόρφωσης και δημιουργία νέας  Script που θα εισαχθεί: Επιλογή _κατάτμησης … Επιλέξτε και προσαρτήσετε την ριζική σας κατάτμηση Επιλέξτε τα απαιτούμενα υποσημεία προσάρτησης Ο bootloader εγκαταστάθηκε επιτυχώς Η δημιουργημένη διαμόρφωση δεν είναι ίδια με την αποθηκευμένη διαμόρφωση κατά την εκκίνηση. Έτσι, ό,τι βλέπετε τώρα μπορεί να μην είναι ό,τι θα δείτε όταν επανεκκινήσετε τον υπολογιστή σας. Για να το διορθώσετε, πατήστε ενημέρωση! Η αποθηκευμένη διαμόρφωση δεν είναι ενήμερη! Αυτά είναι τα σημεία προσάρτησης του αρχείου σας fstab.
Παρακαλούμε επιλέξτε κάθε σχετική με το grub/boot κατάτμηση. Αυτό δεν φαίνεται να είναι ριζικό σύστημα αρχείων (δεν βρέθηκε fstab) Αποπροσάρτηση του επιλεγμένου συστήματος αρχείων Επιλέξατε να χρησιμοποιήσετε άλλη κατάτμηση.
Παρακαλώ σημειώστε: Το os-prober (ανίχνευση λειτουργικού) μπορεί να μη βρει το λειτουργικό που εκτελείτε.
Έτσι εκτελέστε τον Διαμορφωτή του Grub στο σύστημα στόχο
ξανά και αποθηκεύστε τη διαμόρφωση (ή εκτελέστε update-grub/update-burg)
για να ξαναβάλετε την καταχώρηση! Εκκινήσατε το Διαμορφωτή του Grub χρησιμοποιώντας το live CD. Βλέπετε όλες τις καταχωρήσεις (χωρίς διαμόρφωση) όταν εκτελείτε το grub. Αυτό το σφάλμα συμβαίνει (στις περισσότερες περιπτώσεις), όταν δεν έχετε εγκαταστήσει τον διαμορφωτή του grub σωστά. _Συσκευή:  _Επεξεργασία _Αρχείο _Βοήθεια Ε_γκατάσταση στο MBR … Έξο_δος χωρίς αποθήκευση Έ_ξοδος χωρίς ενημέρωση _Αποθήκευση και έξοδος _Ενημέρωση και έξοδος _Προβολή _χρήση άλλης κατάτμησης:  εγκατάσταση του bootloader… είναι ενεργή ετικέτα φόρτωση διαμόρφωσης… όνομα κατάτμηση επαναφόρτωση διαμόρφωσης (ίδιο αποτέλεσμα με επανεκκίνηση εφαρμογής) τύπος Αποτυχία αποπροσάρτησης! ενημέρωση διαμόρφωσης 