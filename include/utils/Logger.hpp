#pragma once
#include <string>
#include <memory>
#include "spdlog/logger.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace E3D {

    /**
     * @brief Interface between spdlog library and E3D
     * 
     */
    class Logger{
    private:
        const static std::string _multisink_logger;
    public:
        /**
         * @brief Construct a new Logger object
         * 
         * @param logPath File path to log file
         */
        Logger(const std::string & logPath);
        /**
         * @brief Get a pointer to the global log object
         * 
         * @return std::shared_ptr<spdlog::logger> 
         */
        inline static std::shared_ptr<spdlog::logger> Getspdlog(){
            return spdlog::get(_multisink_logger);
        };
    };
}