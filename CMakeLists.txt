project(GrubCustomizer)
cmake_minimum_required(VERSION 2.6.2) # may be lower


find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-2.4)
pkg_check_modules(GTHREAD gthread-2.0)
pkg_check_modules(OPENSSL openssl)

link_directories(
    ${GTKMM_LIBRARY_DIRS} ${OPENSSL_LIBRARY_DIRS} )

include_directories(
    ${GTKMM_INCLUDE_DIRS}  )

add_executable(grub-customizer
	src/model/entry.cpp
	src/model/grublistCfg.cpp
	src/view/grublistCfgDlgGtk.cpp
	src/view/grubInstallDlgGtk.cpp
	src/presenter/gtk-client_main.cpp
	src/presenter/grubCustomizer.cpp
	src/presenter/eventListener.cpp
	src/model/libproxyscript_parser.cpp
	src/view/partitionChooserGtk.cpp
	src/model/mountTable.cpp
	src/model/proxy.cpp
	src/model/proxylist.cpp
	src/model/pscriptname_translator.cpp
	src/model/repository.cpp
	src/model/rule.cpp
	src/model/script.cpp
	src/model/deviceDataList.cpp
	src/model/grubEnv.cpp
	src/view/settingsDlgGtk.cpp
	src/model/settings_mng_ds.cpp
	src/model/settingsStore.cpp
	src/model/fbResolutionsGetter.cpp
	src/model/grubInstaller.cpp
	src/model/entryPathBuilderImpl.cpp
	src/view/entryAddDlgGtk.cpp
	src/view/entryEditDlgGtk.cpp
	src/view/aboutDialogGtk.cpp
	src/presenter/glibMutex.cpp
	src/presenter/glibThreadController.cpp
	src/presenter/commonClass.cpp
	src/presenter/streamLogger.cpp
	src/lib/md5.cpp
	src/lib/str_replace.cpp
)


add_executable(grubcfg-proxy
	src/model/entry.cpp
	src/model/rule.cpp
	src/model/script.cpp
	src/model/proxy.cpp
	src/presenter/grubcfg_proxy_main.cpp
	src/presenter/commonClass.cpp
	src/lib/md5.cpp
	src/lib/str_replace.cpp
	# multi:
	src/model/grublistCfg.cpp
	src/model/grubEnv.cpp
	src/model/settingsStore.cpp
	src/model/mountTable.cpp
	src/model/proxylist.cpp
	src/model/repository.cpp
	src/model/pscriptname_translator.cpp
	src/model/libproxyscript_parser.cpp
	src/model/entryPathBuilderImpl.cpp
)

target_link_libraries(grub-customizer 
    ${GTKMM_LIBRARIES} ${GTHREAD_LIBRARIES} ${OPENSSL_LIBRARIES})

target_link_libraries(grubcfg-proxy 
    ${OPENSSL_LIBRARIES})

configure_file ("config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h")

configure_file ("misc/pkexec_policy.in" "${CMAKE_CURRENT_BINARY_DIR}/net.launchpad.danielrichter2007.pkexec.grub-customizer.policy")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/net.launchpad.danielrichter2007.pkexec.grub-customizer.policy
    DESTINATION /usr/share/polkit-1/actions)

install(TARGETS grubcfg-proxy
	DESTINATION lib/grub-customizer)

install(TARGETS grub-customizer 
	DESTINATION bin)

install(FILES misc/manpage.gz
	DESTINATION share/man/man1
	RENAME grub-customizer.1.gz)

#install(FILES misc/copyright
#	DESTINATION share/doc/grub-customizer)

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

install(FILES translation/translation-cs.mo
	DESTINATION share/locale/cs/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-cy.mo
	DESTINATION share/locale/cy/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-hu.mo
	DESTINATION share/locale/hu/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-sk.mo
	DESTINATION share/locale/sk/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-ast.mo
	DESTINATION share/locale/ast/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-kk.mo
	DESTINATION share/locale/kk/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-nl.mo
	DESTINATION share/locale/nl/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-ca.mo
	DESTINATION share/locale/ca/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-ar.mo
	DESTINATION share/locale/ar/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-vi.mo
	DESTINATION share/locale/vi/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-fi.mo
	DESTINATION share/locale/fi/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-lt.mo
	DESTINATION share/locale/lt/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES misc/grub-customizer.desktop
	DESTINATION /usr/share/applications
)


#install icons
install(FILES misc/icons/16x16.png DESTINATION /usr/share/icons/hicolor/16x16/apps RENAME grub-customizer.png)
install(FILES misc/icons/22x22.png DESTINATION /usr/share/icons/hicolor/22x22/apps RENAME grub-customizer.png)
install(FILES misc/icons/24x24.png DESTINATION /usr/share/icons/hicolor/24x24/apps RENAME grub-customizer.png)
install(FILES misc/icons/32x32.png DESTINATION /usr/share/icons/hicolor/32x32/apps RENAME grub-customizer.png)
install(FILES misc/icons/48x48.png DESTINATION /usr/share/icons/hicolor/48x48/apps RENAME grub-customizer.png)
install(FILES misc/icons/64x64.png DESTINATION /usr/share/icons/hicolor/64x64/apps RENAME grub-customizer.png)
install(FILES misc/icons/128x128.png DESTINATION /usr/share/icons/hicolor/128x128/apps RENAME grub-customizer.png)


# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

