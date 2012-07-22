project(GrubCustomizer)
cmake_minimum_required(VERSION 2.6.2) # may be lower


find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-2.4)
pkg_check_modules(GTHREAD gthread-2.0)


link_directories(
    ${GTKMM_LIBRARY_DIRS}  )

include_directories(
    ${GTKMM_INCLUDE_DIRS}  )

add_executable(grub-customizer
	src/entry.cpp
	src/grubconfig.cpp
	src/grubconf_ui_gtk.cpp
	src/gtk-client.cpp
	src/libproxyscript_parser.cpp
	src/liveCDSetupDialog.cpp
	src/mountTable.cpp
	src/proxy.cpp
	src/proxylist.cpp
	src/pscriptname_translator.cpp
	src/repository.cpp
	src/rule.cpp
	src/script.cpp
	src/deviceDataList.cpp
	src/umount_all.cpp
	src/settings_dlg_gtk.cpp
	src/settings_mng_ds.cpp
	src/fbResolutionsGetter.cpp
)


add_executable(grubcfg-proxy
	src/entry.cpp
	src/rule.cpp
	src/script.cpp
	src/proxy.cpp
	src/grubcfg_proxy.cpp
)

target_link_libraries(grub-customizer 
    ${GTKMM_LIBRARIES} ${GTHREAD_LIBRARIES}  )



install(TARGETS grubcfg-proxy
	DESTINATION lib/grub-customizer)

install(TARGETS grub-customizer 
	DESTINATION bin)



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

install(FILES translation/translation-zh_CN.mo
	DESTINATION share/locale/zh_CN/LC_MESSAGES/
	RENAME grub-customizer.mo)


install(FILES grub-customizer.desktop
	DESTINATION /usr/share/applications
)

configure_file ("config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h")

#install icons
install(FILES icons/16x16.png DESTINATION /usr/share/icons/hicolor/16x16/apps RENAME grub-customizer.png)
install(FILES icons/22x22.png DESTINATION /usr/share/icons/hicolor/22x22/apps RENAME grub-customizer.png)
install(FILES icons/24x24.png DESTINATION /usr/share/icons/hicolor/24x24/apps RENAME grub-customizer.png)
install(FILES icons/32x32.png DESTINATION /usr/share/icons/hicolor/32x32/apps RENAME grub-customizer.png)
install(FILES icons/48x48.png DESTINATION /usr/share/icons/hicolor/48x48/apps RENAME grub-customizer.png)
install(FILES icons/64x64.png DESTINATION /usr/share/icons/hicolor/64x64/apps RENAME grub-customizer.png)
install(FILES icons/128x128.png DESTINATION /usr/share/icons/hicolor/128x128/apps RENAME grub-customizer.png)

