# 20.04.2024 10:15:58 Genrated by cbp2cmake V2.4.0 Finale X86 
project(fake-hwclock LANGUAGES C)
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set (CMAKE_INSTALL_PREFIX /usr )
add_executable(fake-hwclock fake-hwclock.c)
install(TARGETS fake-hwclock RUNTIME DESTINATION sbin)
