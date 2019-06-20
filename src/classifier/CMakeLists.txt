cmake_minimum_required(VERSION 3.0)

project(Classifier)
set(CLASSIFIER_VERSION_MAJOE 1)
set(CLASSIFIER_VERSION_MINOR 0)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG)
endif(NOT CMAKE_BUILD_TYPE)

set(WORKSPACE ${CMAKE_CURRENT_LIST_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
cmake_policy(SET CMP0076 NEW)

if(UNIX)
set(EXTENSION_NAME .out)
elseif(WIN)
set(EXTENSION_NAME )
endif(UNIX)

add_compile_definitions(VISUALIZE_DEBUG)

#########################################################################
## Opencv
if(UNIX)
include_directories(/usr/include/opencv4)
elseif(WIN)
message("！！！！ 设置头文件目录")
include_directories(.)
endif(UNIX)

link_libraries(
# opencv_gapi opencv_stitching opencv_aruco opencv_bgsegm opencv_bioinspired opencv_ccalib opencv_dnn_objdetect
# opencv_dpm opencv_face opencv_freetype opencv_fuzzy opencv_hdf opencv_hfs opencv_img_hash
# opencv_line_descriptor opencv_quality opencv_reg opencv_rgbd opencv_saliency opencv_stereo opencv_structured_light
# opencv_phase_unwrapping opencv_superres opencv_optflow opencv_surface_matching opencv_tracking opencv_datasets
# opencv_text opencv_dnn opencv_plot opencv_videostab opencv_video opencv_viz opencv_xfeatures2d
# opencv_shape opencv_ximgproc opencv_xobjdetect opencv_calib3d opencv_features2d
# opencv_videoio opencv_flann opencv_xphoto opencv_photo 
opencv_highgui
opencv_objdetect
opencv_ml
opencv_imgcodecs 
opencv_imgproc
opencv_core
stdc++fs)

set(Headers
    CharInfo.h  
    CharSegment_V3.h  
    csharpImplementations.h  
    PlateCategory_SVM.h  
    PlateChar_SVM.h  
    PlateLocator_V3.h  
    PlateRecognition_V3.h  
    Utilities.h
)

add_definitions(-std=c++17)


#########################################################################
## test_SVM
add_executable(test_SVM${EXTENSION_NAME})
foreach(file ${Headers})
target_sources(test_SVM${EXTENSION_NAME} PUBLIC ${file})
endforeach(file)
target_sources(test_SVM${EXTENSION_NAME} PUBLIC test_SVM.cpp)

#########################################################################
## test_PlateRecognition
add_executable(test_PlateRecognition${EXTENSION_NAME})
foreach(file ${Headers})
    target_sources(test_PlateRecognition${EXTENSION_NAME} PUBLIC ${file})
endforeach(file)
target_sources(test_PlateRecognition${EXTENSION_NAME} PUBLIC test_PlateRecognition.cpp)

#########################################################################
## test_CharSegment_V3
add_executable(test_CharSegment_V3${EXTENSION_NAME})
foreach(file ${Headers})
    target_sources(test_CharSegment_V3${EXTENSION_NAME} PUBLIC ${file})
endforeach(file)
target_sources(test_CharSegment_V3${EXTENSION_NAME} PUBLIC test_CharSegment_V3.cpp)

