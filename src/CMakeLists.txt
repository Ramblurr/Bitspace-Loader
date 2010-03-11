add_definitions(${QT_DEFINITIONS})
include_directories( 
    ${LOADER_INCLUDES}
)
set(loader_SRCS
    TransferItem.cpp
    ConfigDialog.cpp
    FileModel.cpp
    FileDelegate.cpp
    MainWindow.cpp
    main.cpp
    loader_global.cpp
)
set(loader_MOC_SRCS
    TransferItem.h
    ConfigDialog.h
    MainWindow.h
    FileModel.h
)
qt4_wrap_cpp(loader_MOC ${loader_MOC_SRCS})

set(loader_UIS
    ConfigDialog.ui
    MainWindow.ui
)
qt4_wrap_ui(loader_UIS_H ${loader_UIS})

set( loader_RCS
    ../resources/resources.qrc
)
QT4_ADD_RESOURCES( loader_RC_SRCS ${loader_RCS} )

add_executable(loader_bin
    ${loader_SRCS}
    ${loader_MOC_SRCS}
    ${loader_MOC}
    ${loader_UIS_H}
    ${loader_RC_SRCS}
)
set_target_properties(loader_bin PROPERTIES OUTPUT_NAME bsloader)

target_link_libraries( loader_bin ${QT_LIBRARIES} ${BITSPACE_LIBRARY})
