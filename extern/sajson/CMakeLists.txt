add_library(sajson STATIC sajson.cpp sajson.h)
target_include_directories(sajson PUBLIC ..)
target_compile_features(sajson PUBLIC cxx_std_11)
