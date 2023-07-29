#ifndef NET_EXCEPTION_H
#define NET_EXCEPTION_H
#include <format>
#include <stdexcept>

namespace net {

// CONNECTION EXCEPTION

class ConnectionException : public std::runtime_error {
 public:
  ConnectionException(const int error_code)
      : std::runtime_error(std::format("Error code {}", error_code)) {}
};

class UnableToReceiveException : public ConnectionException {
 public:
  UnableToReceiveException(const int error_code)
      : ConnectionException(error_code) {}
};

class StunServerUnavalableException : public std::runtime_error {
 public:
  StunServerUnavalableException() : std::runtime_error("") {}
};

class StunResponseParseException : public std::runtime_error {
 public:
  StunResponseParseException() : std::runtime_error("") {}
};

class UnableToSendException : public ConnectionException {
 public:
  UnableToSendException(const int error_code)
      : ConnectionException(error_code) {}
};

}  // namespace net
#endif  // NET_EXCEPTION_H