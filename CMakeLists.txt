cmake_minimum_required(VERSION 3.10)
project(capsbar C)

add_executable(capsbar WIN32 main.c resource.rc)
target_link_libraries(capsbar shell32 user32 gdi32)

# 最小体积优化
if(MSVC)
    target_compile_options(capsbar PRIVATE /Os /GS- /GL)
    target_link_options(capsbar PRIVATE /OPT:REF /OPT:ICF /LTCG /NODEFAULTLIB:libcmt)
else()
    target_compile_options(capsbar PRIVATE -Os -s)
    target_link_options(capsbar PRIVATE -s -static)
endif()
