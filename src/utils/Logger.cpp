#include "utils/Logger.hpp"

using E3D::Logger;

const std::string Logger::_multisink_logger = "multi_sink";
bool Logger::_isInstantiated = false;
std::shared_ptr<spdlog::logger> Logger::_logger;

Logger::Logger(const std::string &logPath) {
	if (_isInstantiated) {
		return;
	}
	_isInstantiated = true;
	// Info console display
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	console_sink->set_level(spdlog::level::info);
	console_sink->set_pattern("%v");

	// Log file display
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath, true);
	file_sink->set_level(spdlog::level::debug);
	file_sink->set_pattern("%-10l %v");

	// Combining both sinks into a single globally available logger
	std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
	_logger = std::make_shared<spdlog::logger>(_multisink_logger, begin(sinks), end(sinks));
	_logger->set_level(spdlog::level::debug);
}

Logger::Logger() {
	if (_isInstantiated) {
		return;
	}
	_isInstantiated = true;
	// Info console display
	_logger = spdlog::stdout_color_mt(_multisink_logger);
	_logger->set_level(spdlog::level::info);
	_logger->set_pattern("%v");
	// spdlog::register_logger(console);
}