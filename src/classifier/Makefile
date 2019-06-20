INCPATH=-I/usr/include/opencv4
LIBS=\
	-lopencv_gapi -lopencv_stitching -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_dnn_objdetect \
	-lopencv_dpm -lopencv_face -lopencv_freetype -lopencv_fuzzy -lopencv_hdf -lopencv_hfs -lopencv_img_hash \
	-lopencv_line_descriptor -lopencv_quality -lopencv_reg -lopencv_rgbd -lopencv_saliency -lopencv_stereo -lopencv_structured_light \
	-lopencv_phase_unwrapping -lopencv_superres -lopencv_optflow -lopencv_surface_matching -lopencv_tracking -lopencv_datasets \
	-lopencv_text -lopencv_dnn -lopencv_plot -lopencv_videostab -lopencv_video -lopencv_viz -lopencv_xfeatures2d \
	-lopencv_shape -lopencv_ml -lopencv_ximgproc -lopencv_xobjdetect -lopencv_objdetect -lopencv_calib3d -lopencv_features2d \
	-lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_flann -lopencv_xphoto -lopencv_photo -lopencv_imgproc \
	-lopencv_core\
	-lstdc++fs
STANARD=-std=c++17



# %.o: %.cpp csharpImplementations.h
# 	g++ -c $< -g $(INCPATH) $(LIBS) $(STANARD)

# test: test.cpp PlateChar_SVM.o PlateCategory_SVM.o *.h
# 	g++ *.o test.cpp -g -o test.out $(INCPATH) $(LIBS) $(STANARD)

test_svm: test_SVM.cpp *.h
	g++ test_SVM.cpp -g -o test_SVM.out $(INCPATH) $(LIBS) $(STANARD)

test_recognition: test_PlateRecognition.cpp *.h
	g++ test_PlateRecognition.cpp -g -o test_PlateRecognition.out $(INCPATH) $(LIBS) $(STANARD)

test_charsegment: test_CharSegment_V3.cpp *.h
	g++ test_CharSegment_V3.cpp -g -o test_CharSegment_V3.out $(INCPATH) $(LIBS) $(STANARD)

clean:
	rm *.o
	rm *.out