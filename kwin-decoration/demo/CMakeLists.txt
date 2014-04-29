set(oxygen_shadow_demo_SOURCES
    oxygenshadowdemodialog.cpp
    oxygenshadowdemowidget.cpp
    main.cpp)

set(oxygen_shadow_demo_FORMS ui/oxygenshadowdemo.ui)

qt5_wrap_ui(oxygen_shadow_demo_FORMS_HEADERS ${oxygen_shadow_demo_FORMS})

add_executable(oxygen-shadow-demo5 ${oxygen_shadow_demo_SOURCES} ${oxygen_shadow_demo_FORMS_HEADERS})

target_link_libraries(oxygen-shadow-demo5 Qt5::Core Qt5::Gui Qt5::Widgets Qt5::DBus)
target_link_libraries(oxygen-shadow-demo5 KF5::I18n)
target_link_libraries(oxygen-shadow-demo5 oxygenstyle5 )

if(X11_FOUND)
  target_link_libraries(oxygen-shadow-demo5 ${X11_LIBRARIES})
endif()

# install(TARGETS oxygen-shadow-demo5 ${INSTALL_TARGETS_DEFAULT_ARGS})
