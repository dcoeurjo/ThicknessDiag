add_library(${ThicknessDiag_LIB} SHARED
    Event_queue.cpp
    Event_queue_builder.cpp
    Sphere_intersecter.cpp)
target_link_libraries(${ThicknessDiag_LIB}
    ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
