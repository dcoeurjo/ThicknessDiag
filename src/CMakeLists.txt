project(ThicknessDiag-Lib)

add_library(${PROJECT_NAME}
    eventqueuebuilder.cpp
    eventqueue.cpp
    sphereintersecter.cpp)
target_link_libraries(${PROJECT_NAME}
    ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
