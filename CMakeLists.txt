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

configure_file ("config.h.in" "src_dir/config.h")
