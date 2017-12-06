cmake_minimum_required(VERSION 3.2.0)

project(EarleyParser)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "C++11 support has been enabled by default.")

if (NOT WIN32)
	set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
	set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
else()
	ADD_DEFINITIONS(-DNOMINMAX)
	foreach (warning 4996 4018 4244 4267 4506 4800)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd${warning}")
	endforeach(warning)
	#https://msdn.microsoft.com/en-us/library/mt708818.aspx
	# If you want to set both the source character set and the execution character set to UTF-8,
	# you can use the /utf-8 compiler option as a shortcut. It is equivalent to specifying 
	# /source-charset:utf-8 /execution-charset:utf-8 on the command line.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /utf-8")
endif()


set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#before cmake sh co.sh
include_directories(
	"${PROJECT_SOURCE_DIR}/"
)
	
LIST(APPEND SOURCE_FILES 
  EarleyParser.h
  EarleyParser.cpp
)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_NAME}/bin) 
set(LIBRARY_OUTPUT_PATH ${PROJECT_NAME}/lib)
add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries("${PROJECT_NAME}" ${LIBS})
if (build_OffLineParserUtility)
  add_executable(OffLineParserUtility  UseEarleyParser/UseEarleyParser.cpp )
  link_directories(${PROJECT_NAME}/lib)
  TARGET_LINK_LIBRARIES(OffLineParserUtility ${PROJECT_NAME})
endif()
