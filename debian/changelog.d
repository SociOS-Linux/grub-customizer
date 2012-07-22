#!/bin/sh
cat << EOF
grub-customizer (2.2.2-0ubuntu1~ppa1$1) $2; urgency=medium

   * no printing of empty submenus
   * modification detection fixed

 -- Daniel Richter <danielrichter2007@web.de>  Sun, 23 Oct 2011 00:23:57 +0200

grub-customizer (2.2.1-0ubuntu1~ppa1$1) $2; urgency=high

   * crash fixed (Bug #860820)

 -- Daniel Richter <danielrichter2007@web.de>  Tue, 27 Sep 2011 21:34:56 +0200

grub-customizer (2.2-0ubuntu1~ppa1$1) $2; urgency=medium

   * grub 1.99 compatibility
   * new icons

 -- Daniel Richter <danielrichter2007@web.de>  Tue, 13 Sep 2011 23:48:25 +0200

grub-customizer (2.1.2-0ubuntu1~ppa1$1) $2; urgency=medium

   * menu colors fixed
   * multiple loads (results in a chash) prevented
   * empty setting names won't simply be saved anymore
   * settings dialog now better handles the case when entries doesn't exist
   * translation updates

 -- Daniel Richter <danielrichter2007@web.de>  Sat, 26 Mar 2011 17:26:38 +0100

grub-customizer (2.1.1-0ubuntu1~ppa1$1) $2; urgency=medium

   * better conf corruption detection/fix functionality
   * improved view of disabled script entries
   * translation updates

 -- Daniel Richter <danielrichter2007@web.de>  Wed, 02 Feb 2011 23:35:36 +0100

grub-customizer (2.1-0ubuntu1~ppa1$1) $2; urgency=medium

   * based on MVP pattern
   * no modal windows
   * add/remove entry buttons in settings dialog

 -- Daniel Richter <danielrichter2007@web.de>  Sat, 22 Jan 2011 00:04:45 +0100

grub-customizer (2.0.8-0ubuntu1~ppa1$1) $2; urgency=medium

   * french and hungarian translation updated

 -- Daniel Richter <danielrichter2007@web.de>  Tue, 21 Dec 2010 18:09:00 +0100

grub-customizer (2.0.7-0ubuntu1~ppa1$1) $2; urgency=medium

   * spanish and italian translation updated (now complete)

 -- Daniel Richter <danielrichter2007@web.de>  Wed, 08 Dec 2010 20:45:42 +0100

grub-customizer (2.0.6-0ubuntu1~ppa2$1) $2; urgency=medium

   * Brasilian translation updated (now complete)

 -- Daniel Richter <danielrichter2007@web.de>  Wed, 08 Dec 2010 20:45:42 +0100

grub-customizer (2.0.6-0ubuntu1~ppa1$1) $2; urgency=medium

   * first upstream release (LP: #684977)
   * lintian and compiler warnings removed
   * manpage created
   * using su-to-root instead of gksu in the menu entry

 -- Daniel Richter <danielrichter2007@web.de>  Wed, 08 Dec 2010 20:45:42 +0100

grub-customizer (2.0.5-0ubuntu1~ppa1$1) $2; urgency=medium

   * translations updated

 -- Daniel Richter <danielrichter2007@web.de>  Tue, 30 Nov 2010 18:44:45 +0100

grub-customizer (2.0.4-0ubuntu1~ppa1$1) $2; urgency=medium

   * chineese translation added, frensh and turkish translation updated
   * fixed: timeout not settable when "show menu" not selected
   * renamed "black" to "transparent" in background selection

 -- Daniel Richter <danielrichter2007@web.de>  Sun, 28 Nov 2010 21:17:47 +0100

grub-customizer (2.0.3-0ubuntu1~ppa1$1) $2; urgency=medium

   * fixed: crash when choosing an img which can not be displayed by gtk
   * polish translation updated

 -- Daniel Richter <danielrichter2007@web.de>  Tue, 23 Nov 2010 21:04:34 +0100

grub-customizer (2.0.2-0ubuntu1~ppa1$1) $2; urgency=medium

   * translations (Greek and Ukrainian) updated

 -- Daniel Richter <danielrichter2007@web.de>  Mon, 22 Nov 2010 21:44:56 +0100

grub-customizer (2.0.1-0ubuntu1~ppa1$1) $2; urgency=medium

   * Bulgarian and Turkish translation updated
   * some partition chooser bugfixes

 -- Daniel Richter <danielrichter2007@web.de>  Mon, 22 Nov 2010 00:01:43 +0100

grub-customizer (2.0-0ubuntu1~ppa1$1) $2; urgency=medium

   * settings manager included

 -- Daniel Richter <danielrichter2007@web.de>  Sun, 21 Nov 2010 17:42:31 +0100

grub-customizer (1.6.4-0ubuntu1~ppa1$1) $2; urgency=low

   * translations updated

 -- Daniel Richter <danielrichter2007@web.de>  Tue, 16 Nov 2010 20:22:00 +0200

grub-customizer (1.6.3-0ubuntu1~ppa1$1) $2; urgency=low

   * translations updated

 -- Daniel Richter <danielrichter2007@web.de>  Sat, 06 Nov 2010 09:00:00 +0200

grub-customizer (1.6.2-0ubuntu1~ppa1$1) $2; urgency=low

   * greek translation updated
   * translators added to the about dialog

 -- Daniel Richter <danielrichter2007@web.de>  Sat, 22 Oct 2010 11:43:00 +0200

grub-customizer (1.6.1-0ubuntu1~ppa1$1) $2; urgency=medium

   * fixed burg mode indentfier (has not been updated)
   * translations included

 -- Daniel Richter <danielrichter2007@web.de>  Sat, 22 Oct 2010 11:43:00 +0200

grub-customizer (1.6-0-0ubuntu1~ppa1$1) $2; urgency=medium
 
   * bug #661304 fixed (multiple scripts using the same basename will not be overwritten anymore)
   * (new entries) will be displayed on uncustomized scripts too (the old behaviour was to fuzzy)
   * the test, if a script requires a proxy, is now solved dynamically (the previous behaviour was: when it's modified, set proxy_required)
   * the live cd setup assistant is now (renamed to partition chooser) accessible after loading (by menubutton click) or if no bootloader found too
   * the toolbar button for saving the configuration is now marked as is_important (will be displayed larger in most cases)
 
 -- Daniel Richter <danielrichter2007@web.de>  Sat, 22 Oct 2010 11:43:00 +0200

EOF
