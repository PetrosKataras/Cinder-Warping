if ( NOT TARGET Cinder-Warping )
	get_filename_component( CINDER_WARPING_PATH "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE )

	set( CINDER_WARPING_INCLUDES
		${CINDER_WARPING_PATH}/include
	)
	set( CINDER_WARPING_SOURCES
		${CINDER_WARPING_PATH}/src/Warp.cpp
		${CINDER_WARPING_PATH}/src/WarpBilinear.cpp
		${CINDER_WARPING_PATH}/src/WarpPerspective.cpp
		${CINDER_WARPING_PATH}/src/WarpPerspectiveBilinear.cpp
	)

	add_library( Cinder-Warping ${CINDER_WARPING_SOURCES} )

	#target_compile_options( Cinder-Warping PUBLIC "-std=c++11" )

	target_include_directories( Cinder-Warping PUBLIC "${CINDER_WARPING_INCLUDES}" )
	target_include_directories( Cinder-Warping SYSTEM BEFORE PUBLIC "${CINDER_PATH}/include" )

	if( NOT TARGET cinder )
		include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
		find_package( cinder REQUIRED PATHS
			"${CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
			"$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )
	endif()

	target_link_libraries( Cinder-Warping PRIVATE cinder )
	set_target_properties(Cinder-Warping
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)
endif()
