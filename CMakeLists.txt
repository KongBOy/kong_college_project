cmake_minimum_required(VERSION 3.0.0)

set(EXPORT_COMPILE_COMMANDS ON)

project(project_rebuild)

# 把 build 結果放到 指定的資料夾
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_SOURCE_DIR}/Resource")

find_package(OpenCV REQUIRED "C:/Users/VAIO/Desktop/OpenCV/opencv231_2_build")
set(CMAKE_BUILD_TYPE Release)
message(STATUS "(可省, 看一下變數內容而已)OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}" )
message(STATUS "(可省, 看一下變數內容而已)OpenCV_LIBS        : ${OpenCV_LIBS}" )
message(STATUS "(可省, 看一下變數內容而已)CMAKE_BUILD_TYPE   : ${CMAKE_BUILD_TYPE}" )

# String_tool_lib
add_library               (String_tool_lib String_tool_lib/string_tools.cpp)
target_include_directories(String_tool_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/String_tool_lib/include )


# Recognition_lib
add_library(Recognition_lib Recognition_lib/Recognition.cpp

Recognition_lib/preprocess/src/preprocess_0_bar_roi.cpp
                            Recognition_lib/preprocess/src/preprocess_0_hough_tool.cpp
                            Recognition_lib/preprocess/src/preprocess_0_watch_hough_line.cpp
                            Recognition_lib/preprocess/src/preprocess_1_warp_straight_rough.cpp
                            Recognition_lib/preprocess/src/preprocess_2_binary.cpp
                            Recognition_lib/preprocess/src/preprocess_3_horizon_map_to_find_line.cpp
                            Recognition_lib/preprocess/src/preprocess_4_distance_detect.cpp
                            Recognition_lib/preprocess/src/preprocess_5_find_staff.cpp
                            Recognition_lib/preprocess/src/preprocess_6_find_head.cpp
                            Recognition_lib/preprocess/src/preprocess_6_reduce_line.cpp
                            Recognition_lib/preprocess/src/preprocess_7_cut_according_staff.cpp

                            Recognition_lib/recognition/src/recognition_0_all_head.cpp
                            Recognition_lib/recognition/src/recognition_0_array_tools.cpp
                            Recognition_lib/recognition/src/recognition_0_debug_tools.cpp
                            Recognition_lib/recognition/src/recognition_0_vertical_map_to_speed_up.cpp
                            Recognition_lib/recognition/src/recognition_1_find_all_maybe_head_method_2.cpp
                            Recognition_lib/recognition/src/recognition_1_find_all_maybe_head.cpp
                            Recognition_lib/recognition/src/recognition_2_a_head_charactaristic.cpp
                            Recognition_lib/recognition/src/recognition_2_b_head_recheck.cpp
                            Recognition_lib/recognition/src/recognition_3_a_find_vertical_bar.cpp
                            Recognition_lib/recognition/src/recognition_3_b_find_time_bar.cpp
                            Recognition_lib/recognition/src/recognition_3_c_merge_head_and_time.cpp
                            Recognition_lib/recognition/src/recognition_4_find_picth.cpp

                            Recognition_lib/recognition/src/UI_loading.cpp
                            )
target_include_directories(Recognition_lib PUBLIC ${OpenCV_INCLUDE_DIRS} 
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/Recognition_lib/include
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/Recognition_lib/recognition/include
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/Recognition_lib/preprocess/include
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/String_tool_lib/include
                                                  )
target_link_libraries     (Recognition_lib PUBLIC ${OpenCV_LIBS} 
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/Recognition_lib/recognition/include
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/Recognition_lib/preprocess/include
                                                  String_tool_lib)

# main
add_executable            (project_rebuild main.cpp src/UserEnterFile.cpp)
target_include_directories(project_rebuild PUBLIC ${OpenCV_INCLUDE_DIRS}
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/include
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/Recognition_lib/include
                                                  ${CMAKE_CURRENT_SOURCE_DIR}/Recognition_lib/recognition/include
                                                  "C:/Program Files (x86)/CodeBlocks/MinGW/include"
                                                  )
target_link_libraries     (project_rebuild PUBLIC ${OpenCV_LIBS}
                                                  Recognition_lib
                                                  "C:/Program Files (x86)/CodeBlocks/MinGW/lib/libwinmm.a"
                                                  )