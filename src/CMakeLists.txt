SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wall -g -ggdb -Wno-deprecated")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -Wno-deprecated")

SET(SOURCES 
	APD.cpp
	BP.cpp
	Collection.cpp
	CommandLineOptions.cpp
	fft.cpp
	HZCRR.cpp
	LSTER.cpp
	MyFFT.cpp
	NFR.cpp
	ReadFeature.cpp
	realvec.cpp
	Rms.cpp
	RTPD.cpp
	SFR.cpp
	Spectrum.cpp
	STE.cpp
	svm.cpp
	SVMClassifier.cpp
	WavRead.cpp
	WekaData.cpp
#	trainSN.cpp
#	trainMB.cpp
	trainSMB.cpp
#	trainPN.cpp
#	recognizer.cpp
)

	
STRING(REGEX REPLACE ".cpp" ".h" HEADERS "${SOURCES}")

MESSAGE(STATUS "Libraries ....")
#SET(MFCC_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/libmfcc.a")
#MESSAGE(STATUS "MFCC Library:${MFCC_LIBRARIES}")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
ADD_EXECUTABLE(${TARGETNAME} ${SOURCES})

#TARGET_LINK_LIBRARIES(${TARGETNAME} ${MFCC_LIBRARIES})

