set(
 LLVM_LINK_COMPONENTS 
 support
)

add_clang_executable(
 example 
 Example.cpp
 Rplsmt.cpp)

target_link_libraries(
 example
 clangTooling
)
