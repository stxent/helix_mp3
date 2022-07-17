# Create a list of source files

list(APPEND SOURCE_FILES "mp3dec.c")
list(APPEND SOURCE_FILES "mp3tabs.c")
list(APPEND SOURCE_FILES "real/bitstream.c")
list(APPEND SOURCE_FILES "real/buffers.c")
list(APPEND SOURCE_FILES "real/dct32.c")
list(APPEND SOURCE_FILES "real/dequant.c")
list(APPEND SOURCE_FILES "real/dqchan.c")
list(APPEND SOURCE_FILES "real/huffman.c")
list(APPEND SOURCE_FILES "real/hufftabs.c")
list(APPEND SOURCE_FILES "real/imdct.c")
list(APPEND SOURCE_FILES "real/polyphase.c")
list(APPEND SOURCE_FILES "real/scalfact.c")
list(APPEND SOURCE_FILES "real/stproc.c")
list(APPEND SOURCE_FILES "real/subband.c")
list(APPEND SOURCE_FILES "real/trigtabs.c")

# Create object library

add_library(helix_mp3_fixpnt OBJECT ${SOURCE_FILES})
target_include_directories(helix_mp3_fixpnt PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/real")
target_include_directories(helix_mp3_fixpnt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/pub")

if(HELIX_MP3_STATIC)
    target_compile_definitions(helix_mp3_fixpnt PRIVATE -DUSE_STATIC=1)
else()
    target_compile_definitions(helix_mp3_fixpnt PRIVATE -DUSE_STATIC=0)
endif()
