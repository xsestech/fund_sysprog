#include <filesystem>
#include <utility>
#include <not_implemented.h>
#include "../include/client_logger_builder.h"
#include <not_implemented.h>

using json = nlohmann::json;
namespace fs = std::filesystem;

logger_builder& client_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity) &
{
  _output_streams[severity].first.emplace_front(stream_file_path);
  return *this;
}

logger_builder& client_logger_builder::add_console_stream(
    const logger::severity severity) &
{
  _output_streams[severity].second = true;
  return *this;
}

logger_builder& client_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path) &
{
  const fs::path conf_dir = configuration_path;
  const fs::path conf_filename = configuration_file_path;
  const fs::path full_path = conf_dir / conf_filename;
  std::ifstream conf_stream(full_path);
  if (!conf_stream.is_open()) {
    throw std::runtime_error("Could not open configuration file");
  }
  for (json j = json::parse(conf_stream); const auto &item : j.items()) {
    logger::severity sev = string_to_severity(item.key());
    parse_severity(sev, item.value());
  }
  return *this;
}

logger_builder& client_logger_builder::clear() &
{
  _output_streams.clear();
  return *this;
}

std::unique_ptr<logger> client_logger_builder::build() const
{
  return std::unique_ptr<logger>(new client_logger(_output_streams, _format));
}

logger_builder& client_logger_builder::set_format(const std::string &format) &
{
  _format = format;
  return *this;
}

void client_logger_builder::parse_severity(logger::severity sev, nlohmann::json& j)
{
  for (auto &stream_string: j) {
    if (stream_string == "stdin") {
      add_console_stream(sev);
      continue;
    }
    add_file_stream(stream_string, sev);
  }
}

logger_builder& client_logger_builder::set_destination(const std::string &format) &
{
    throw not_implemented("logger_builder *client_logger_builder::set_destination(const std::string &format)", "invalid call");
}
