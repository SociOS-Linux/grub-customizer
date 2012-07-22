project(GrubCustomizer)
cmake_minimum_required(VERSION 2.8) # may be lower


find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-2.4) # look into FindPkgConfig.cmake, 
                                                                # it contains documentation
# Now the variables GTKMM_INCLUDE_DIRS, GTKMM_LIBRARY_DIRS and GTKMM_LIBRARIES 
# contain what you expect 

add_subdirectory(src_dir)

install(FILES translation/german.mo
	DESTINATION share/locale/de/LC_MESSAGES/
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

