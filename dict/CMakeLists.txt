set(ROOT_DICT_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")

set(STRUCTURES_SOURCE "${TOP_DIRECTORY}/dict/source/Structures.cpp")
set(STRUCTURES_HEADER "${TOP_DIRECTORY}/dict/include/Structures.hpp")
set(LINKDEF_FILE "${TOP_DIRECTORY}/dict/include/LinkDef.h")

set(CINT_OUTPUT_FILE "${ROOT_DICT_BUILD_DIR}/${DICTIONARY_PREFIX}.cpp")
set(UNREG_OUTPUT_FILE "${ROOT_DICT_BUILD_DIR}/${DICTIONARY_PREFIX}_mod.cpp")

add_custom_target(GenerateDict ALL DEPENDS ${STRUCTURES_SOURCE} ${STRUCTURES_HEADER} ${LINKDEF_FILE} ${CINT_OUTPUT_FILE})

#Copy the new Structures file to the top-level include directory.
#add_custom_command(OUTPUT ${COPIED_STRUCTURES_HEADER} COMMAND cp ${STRUCTURES_HEADER} ${COPIED_STRUCTURES_HEADER} DEPENDS ${STRUCTURES_HEADER} VERBATIM)

#Use rootcint to generate dictionary source file.
add_custom_command(OUTPUT ${CINT_OUTPUT_FILE} COMMAND ${ROOTCINT_EXECUTABLE} -f ${CINT_OUTPUT_FILE} -c ${STRUCTURES_HEADER} ${LINKDEF_FILE} DEPENDS ${STRUCTURES_HEADER} ${LINKDEF_FILE} VERBATIM)

#Add the sources to the library.
add_library(DictObjects OBJECT ${STRUCTURES_SOURCE} ${CINT_OUTPUT_FILE})

#Create static library.
add_library(${DICTIONARY_PREFIX}Static STATIC $<TARGET_OBJECTS:DictObjects>)
