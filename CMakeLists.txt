cmake_minimum_required(VERSION 3.20)

project(LabelPrinterApp LANGUAGES CXX)

add_executable(LabelPrinterApp
    src/main.cpp
    src/ZebraPrinter.cpp
)

target_compile_features(LabelPrinterApp PRIVATE cxx_std_17)
target_include_directories(LabelPrinterApp PRIVATE include)

if (WIN32)
    target_link_libraries(LabelPrinterApp PRIVATE Winspool)
else()
    message(FATAL_ERROR "LabelPrinterApp requires Windows WinSpool APIs.")
endif()
