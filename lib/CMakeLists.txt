add_library(${ThicknessDiag_LIB} SHARED
    Handle.cpp
    Event_queue.cpp
    Event_queue_builder.cpp
    Sphere_intersecter.cpp)
target_link_libraries(${ThicknessDiag_LIBRARIES})
