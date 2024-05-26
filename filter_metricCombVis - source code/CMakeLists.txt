set(SOURCES ${sources} plugin.cpp)
set(HEADERS ${headers} plugin.h)

add_meshlab_plugin(metricCombVis ${SOURCES} ${HEADERS})

