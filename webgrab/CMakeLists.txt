find_package(Qt6 
    REQUIRED COMPONENTS 
    Svg Pdf 
    WebChannel WebEngineCore WebEngineWidgets Positioning)

target_link_libraries(webgrab PRIVATE Qt6::Widgets Qt6::PrintSupport
    Qt6::Svg  Qt6::Pdf Qt6::WebEngineCore  Qt6::WebEngineWidgets)

target_sources(webgrab PRIVATE webgrab/Options.h)
target_sources(webgrab PRIVATE webgrab/Options.cpp)

target_sources(webgrab PRIVATE webgrab/PrinterWE.h)
target_sources(webgrab PRIVATE webgrab/webgrabWE.cpp)
target_sources(webgrab PRIVATE webgrab/PrinterWE.cpp)

