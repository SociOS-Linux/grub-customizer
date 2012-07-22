project(GrubCustomizer)
cmake_minimum_required(VERSION 2.8) # may be lower


find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-2.4) # look into FindPkgConfig.cmake, 
                                                                # it contains documentation
# Now the variables GTKMM_INCLUDE_DIRS, GTKMM_LIBRARY_DIRS and GTKMM_LIBRARIES 
# contain what you expect 

add_subdirectory(src_dir)


install(FILES translation/translation-de.mo
	DESTINATION share/locale/de/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-bg.mo
	DESTINATION share/locale/bg/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-el.mo
	DESTINATION share/locale/el/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-gl.mo
	DESTINATION share/locale/gl/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-hr.mo
	DESTINATION share/locale/hr/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-pl.mo
	DESTINATION share/locale/pl/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-pt_BR.mo
	DESTINATION share/locale/pt_BR/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-ru.mo
	DESTINATION share/locale/ru/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-tr.mo
	DESTINATION share/locale/tr/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-uk.mo
	DESTINATION share/locale/uk/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-it.mo
	DESTINATION share/locale/it/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-es.mo
	DESTINATION share/locale/es/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-fr.mo
	DESTINATION share/locale/fr/LC_MESSAGES/
	RENAME grub-customizer.mo)


install(FILES grub-customizer.desktop
	DESTINATION /usr/share/applications
)

configure_file ("config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/src_dir/config.h")

#install icons
install(FILES icons/16x16.png DESTINATION /usr/share/icons/hicolor/16x16/apps RENAME grub-customizer.png)
install(FILES icons/22x22.png DESTINATION /usr/share/icons/hicolor/22x22/apps RENAME grub-customizer.png)
install(FILES icons/24x24.png DESTINATION /usr/share/icons/hicolor/24x24/apps RENAME grub-customizer.png)
install(FILES icons/32x32.png DESTINATION /usr/share/icons/hicolor/32x32/apps RENAME grub-customizer.png)
install(FILES icons/48x48.png DESTINATION /usr/share/icons/hicolor/48x48/apps RENAME grub-customizer.png)
install(FILES icons/64x64.png DESTINATION /usr/share/icons/hicolor/64x64/apps RENAME grub-customizer.png)
install(FILES icons/128x128.png DESTINATION /usr/share/icons/hicolor/128x128/apps RENAME grub-customizer.png)

