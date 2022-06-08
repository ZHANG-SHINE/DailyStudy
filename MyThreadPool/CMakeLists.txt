project(mythreadpool)

add_executable(threadpool main.cpp MyThreadPool.cpp)
target_link_libraries(threadpool -lpthread)