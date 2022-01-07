//Note: include glad after file 
#pragma once
#include"spdlog/spdlog.h"



#define MCLONE_DEFAULT_LOGGER_NAME "MCLONELogger"


#define MCLONE_BREAK() __debugbreak();


#ifndef MCLONE_CONFIG_RELEASE
#define MCLONE_TRACE(...)	 if(spdlog::get(MCLONE_DEFAULT_LOGGER_NAME)!=nullptr) {spdlog::get(MCLONE_DEFAULT_LOGGER_NAME)->trace(__VA_ARGS__);}
#define MCLONE_DEBUG(...)	 if(spdlog::get(MCLONE_DEFAULT_LOGGER_NAME)!=nullptr) {spdlog::get(MCLONE_DEFAULT_LOGGER_NAME)->debug(__VA_ARGS__);}
#define MCLONE_INFO(...)	 if(spdlog::get(MCLONE_DEFAULT_LOGGER_NAME)!=nullptr) {spdlog::get(MCLONE_DEFAULT_LOGGER_NAME)->info(__VA_ARGS__);}
#define MCLONE_WARN(...)	 if(spdlog::get(MCLONE_DEFAULT_LOGGER_NAME)!=nullptr) {spdlog::get(MCLONE_DEFAULT_LOGGER_NAME)->warn(__VA_ARGS__);}
#define MCLONE_ERROR(...)	 if(spdlog::get(MCLONE_DEFAULT_LOGGER_NAME)!=nullptr) {spdlog::get(MCLONE_DEFAULT_LOGGER_NAME)->error(__VA_ARGS__);}
#define MCLONE_FATAL(...)	 if(spdlog::get(MCLONE_DEFAULT_LOGGER_NAME)!=nullptr) {spdlog::get(MCLONE_DEFAULT_LOGGER_NAME)->critical(__VA_ARGS__);}
#define MCLONE_ASSERT(x,msg) if((x)) {} else {MCLONE_FATAL("ASSERT - {}\n\t{}\n\tfile: {}\n\ton line: {}",#x,msg,__FILE__,__LINE__); MCLONE_BREAK(); }
#else
//disable logging in release builds
#define MCLONE_TRACE(...) (void)0
#define MCLONE_DEBUG(...) (void)0
#define MCLONE_INFO(...)  (void)0
#define MCLONE_WARN(...)  (void)0
#define MCLONE_ERROR(...) (void)0
#define MCLONE_FATAL(...) (void)0
#endif
