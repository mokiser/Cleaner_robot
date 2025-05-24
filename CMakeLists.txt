cmake_minimum_required(VERSION 3.12)
project(_MQTT)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск необходимых библиотек
set(OpenCV_DIR "/usr/lib64/cmake/OpenCV4")
find_package(PkgConfig REQUIRED)
pkg_check_modules(MOSQUITTO REQUIRED IMPORTED_TARGET libmosquitto)
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MOSQUITTO REQUIRED libmosquitto)

# Настройка исполняемого файла
add_executable(cvcontroller
    cvoperator.cpp
)

# Подключение библиотек
target_include_directories(cvcontroller PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${MOSQUITTO_INCLUDE_DIRS}
PkgConfig::MOSQUITTO
)

target_link_libraries(cvcontroller
    ${OpenCV_LIBS}
    ${MOSQUITTO_LIBRARIES}
)

# Установка выходного файла
install(TARGETS cvcontroller DESTINATION bin)
