project(qtlabors)

set(QTLABORS_SOURCES
    main.cpp
    color.cpp
    EventProxy.cpp
    GridView.cpp
    GridViewDelegate.cpp
    GridViewHeader.cpp
    LaborColumn.cpp
    MainWindow.cpp
    SkillColumn.cpp
    UnitModel.cpp
    ViewColumn.cpp
)

qt5_wrap_cpp(QTLABORS_MOC_SOURCES
    EventProxy.h
    GridView.h
    GridViewDelegate.h
    GridViewHeader.h
    MainWindow.h
    UnitModel.h
    ViewColumn.h
)

qt5_wrap_ui(QTLABORS_UI_SOURCES
    MainWindow.ui
)

DFHACK_PLUGIN(qtlabors
    ${QTLABORS_SOURCES} ${QTLABORS_MOC_SOURCES} ${QTLABORS_UI_SOURCES}
    LINK_LIBRARIES Qt5::Core Qt5::Widgets)
target_include_directories(qtlabors PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
