#include <filesystem>
#include <utility>
#include <not_implemented.h>
#include "../include/client_logger_builder.h"
#include <not_implemented.h>

using namespace nlohmann;

logger_builder& client_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity) &
{
  _output_streams[severity].first.emplace_front(stream_file_path);
}

logger_builder& client_logger_builder::add_console_stream(
    logger::severity severity) &
{
  _output_streams[severity].second = true;
}

logger_builder& client_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path) &
{
    throw not_implemented("logger_builder& client_logger_builder::transform_with_configuration(std::string const &,std::string const &) &", "your code should be here...");
}

logger_builder& client_logger_builder::clear() &
{
  _output_streams.clear();
}

std::unique_ptr<logger> client_logger_builder::build() const
{
  return std::unique_ptr<logger>(new client_logger(_output_streams, _format));
}

logger_builder& client_logger_builder::set_format(const std::string &format) &
{
  _format = format;
}

void client_logger_builder::parse_severity(logger::severity sev, nlohmann::json& j)
{
    throw not_implemented("void client_logger_builder::parse_severity(logger::severity, nlohmann::json&)", "your code should be here...");
}

logger_builder& client_logger_builder::set_destination(const std::string &format) &
{
    throw not_implemented("logger_builder *client_logger_builder::set_destination(const std::string &format)", "invalid call");
}
