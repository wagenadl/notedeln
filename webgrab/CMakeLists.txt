find_package(Qt6 COMPONENTS Svg WebEngineCore WebEngineWidgets Pdf REQUIRED)  

target_link_libraries(webgrab Qt6::Widgets)
target_link_libraries(webgrab Qt6::PrintSupport)
target_link_libraries(webgrab Qt6::Svg)
target_link_libraries(webgrab Qt6::Pdf)
target_link_libraries(webgrab Qt6::WebEngineCore)
target_link_libraries(webgrab Qt6::WebEngineWidgets)

target_sources(webgrab PRIVATE webgrab/Options.h)
target_sources(webgrab PRIVATE webgrab/Options.cpp)

target_sources(webgrab PRIVATE webgrab/PrinterWE.h)
target_sources(webgrab PRIVATE webgrab/webgrabWE.cpp)
target_sources(webgrab PRIVATE webgrab/PrinterWE.cpp)

