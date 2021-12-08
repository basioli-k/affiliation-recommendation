add_executable(random_katz main.cpp)
target_link_libraries(random_katz LINK_PUBLIC fmt)

set_target_properties(
  random_katz
    PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS ON
    CXX_STANDARD_REQUIRED ON
  )