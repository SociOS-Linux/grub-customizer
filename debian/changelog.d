#!/bin/sh
cat << EOF
grub-customizer (2.0.5-0ubuntu1~ppa1$1) $2; urgency=medium

   * settings manager included
   * 2.0.1: Bulgarian and Turkish translation updated
   * 2.0.1: some partition chooser bugfixes
   * 2.0.2: translations (Greek and Ukrainian) updated
   * 2.0.3: fixed: when choosing an image which can not be displayed by gtk, the whole application crashes (because of an uncaught examption)
   * 2.0.3: polish translation updated
   * 2.0.4: chineese translation added, frensh and turkish translation updated
   * 2.0.4: fixed: timeout not settable when "show menu" not selected
   * 2.0.4: renamed "black" to "transparent" in background selection (highlight and normal)
   * 2.0.5: translations updated

 -- Daniel Richter <danielrichter2007@web.de>  `date -R`

EOF
