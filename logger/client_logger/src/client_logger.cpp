#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include "../include/client_logger.h"
#include <not_implemented.h>

constexpr size_t kInitRefCount = 1;

std::unordered_map<std::string, std::pair<size_t, std::ofstream>> client_logger::refcounted_stream::_global_streams;


logger &client_logger::log(
    const std::string &message,
    const logger::severity severity) &{
  const std::string formatted_text = make_format(message, severity);
  auto it = _output_streams.find(severity);
  auto& [streams, stdout_flag] = it->second;
  if (it->second.second) {
    std::cout << formatted_text;
  }
  for (const auto& stream : streams) {
    *stream._stream.second << formatted_text;
  }
  return *this;
}

std::string client_logger::make_format(const std::string &message, severity sev) const {
   std::string result;
   for (size_t i = 0; i < _format.length() - 1; ++i) {
     if (_format.at(i) == '%') {
         auto flag = char_to_flag(_format.at(i + 1));
         switch (flag) {
            case flag::DATE:
              result += current_date_to_string();
              break;
            case flag::TIME:
              result += current_time_to_string();
              break;
            case flag::SEVERITY:
              result += severity_to_string(sev);
              break;
            case flag::MESSAGE:
              result += message;
              break;
            default:
              throw std::invalid_argument("Invalid format specifier");
         }
         ++i;
      } else {
         result += _format.at(i);
     }
   }
  result += '\n';
  return result;
}

client_logger::client_logger(
    const std::unordered_map<logger::severity, std::pair<std::forward_list<refcounted_stream>, bool>> &streams,
    std::string format) {
  _output_streams = streams;
  _format = std::move(format);
}

client_logger::flag client_logger::char_to_flag(char c) noexcept {
  switch (c) {
    case 'd':
      return flag::DATE;
    case 't':
      return flag::TIME;
    case 's':
      return flag::SEVERITY;
    case 'm':
      return flag::MESSAGE;
    default:
      return flag::NO_FLAG;
  }
}
void client_logger::deepcopy(const client_logger &other){
  _format = other._format;

  for (const auto& [severity, stream_pair]: other._output_streams) {
    auto &new_pair = _output_streams[severity];

    new_pair.second = stream_pair.second;

    for (const auto &stream : stream_pair.first) {
      new_pair.first.push_front(stream);
    }
    new_pair.first.reverse(); // Keep order
  }
}

client_logger::client_logger(const client_logger &other) {
  deepcopy(other);
}

client_logger &client_logger::operator=(const client_logger &other) {
  if (this != &other) {
    deepcopy(other);
  }
  return *this;
}

client_logger::client_logger(client_logger &&other) noexcept {
  _format = std::move(other._format);
  _output_streams = std::move(_output_streams);
}

client_logger &client_logger::operator=(client_logger &&other) noexcept {
  _format = std::move(other._format);
  _output_streams = std::move(_output_streams);
  return *this;
}

client_logger::~client_logger() noexcept = default;

client_logger::refcounted_stream::refcounted_stream(const std::string &path) {
  if (path.empty()) {
    throw std::invalid_argument("Path cannot be empty");
  }
  _stream.first = path;

  auto it = _global_streams.find(path);

  if (it == _global_streams.end()) {
    std::ofstream stream(path, std::ios::app);
    if (!stream.is_open()) {
      throw std::runtime_error("Could not open " + path);
    }
    auto [new_it, success] = _global_streams.emplace(
        path,
        std::make_pair(kInitRefCount, std::move(stream))
    );
    if (!success) {
      throw std::runtime_error("Failed to push stream to path");
    }
    _stream.second = &new_it->second.second;
    return;
  }

  it->second.first++;
  _stream.second = &it->second.second;

}

void client_logger::refcounted_stream::copy_from_stream(const client_logger::refcounted_stream &oth) {
  _stream = oth._stream;
  auto it = _global_streams.find(_stream.first);
  it->second.first++;
}

client_logger::refcounted_stream::refcounted_stream(const client_logger::refcounted_stream &oth) {
  copy_from_stream(oth);
}

void client_logger::refcounted_stream::release() {
  auto it = _global_streams.find(_stream.first);
  if (it != _global_streams.end()) {
    if (--it->second.first == 0) {
      it->second.second.close();
      _global_streams.erase(it);
    }
    _stream.second = nullptr;
  }
}

client_logger::refcounted_stream &
client_logger::refcounted_stream::operator=(const client_logger::refcounted_stream &oth) {
  if (_stream.first == oth._stream.first) {
    return *this;
  }
  release();
  copy_from_stream(oth);
  return *this;
}

client_logger::refcounted_stream::refcounted_stream(client_logger::refcounted_stream &&oth) noexcept: _stream(std::move(
    oth._stream)) {
}

client_logger::refcounted_stream &client_logger::refcounted_stream::operator=(client_logger::refcounted_stream &&oth) noexcept {
  if (this != &oth) {
    release();
    _stream = std::move(oth._stream);
  }
  return *this;
}

client_logger::refcounted_stream::~refcounted_stream() {
  release();
}

void client_logger::refcounted_stream::open() {
  throw not_implemented(__func__ , "I don't need this shit");
}
