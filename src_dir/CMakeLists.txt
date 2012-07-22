link_directories(
    ${GTKMM_LIBRARY_DIRS}  )

include_directories(
    ${GTKMM_INCLUDE_DIRS}  )

add_executable(../grub-customizer
	confparser.cpp
	grubconfig.cpp
	grubconf_ui_gtk.cpp
	gtk-client.cpp
	libproxy.cpp
	libproxyscript_parser.cpp
)

add_executable(../grubcfg-proxy
	confparser.cpp
	libproxy.cpp
	libproxyscript_parser.cpp
	grubcfg_proxy.cpp
)

target_link_libraries(../grub-customizer 
    ${GTKMM_LIBRARIES}  )



install(TARGETS ../grubcfg-proxy
	DESTINATION lib/grub-customizer)

install(TARGETS ../grub-customizer 
	DESTINATION bin)


