#pragma once

#include <string>
#include <cstring>
#include <stdexcept>

namespace argparse
{
struct usage_error : std::runtime_error
{
  explicit usage_error(const std::string& msg)
    : runtime_error(msg)
  {
  }
};

template <std::size_t N>
std::size_t strlen(const char (&)[N])
{
  return N - 1;
}

char** parseArgument(char** argv, size_t token_offset, int& v)
{
  const char* startptr = *argv + token_offset;
  char* argend = *argv + std::strlen(*argv);

  if (startptr == argend)
  {
    startptr = *++argv;
    argend = *argv + std::strlen(*argv);
  }

  char* endptr = nullptr;
  v = std::strtol(startptr, &endptr, 10);

  if (endptr < argend)
    throw usage_error("expected integer argument");

  return argv;
}

char** parseArgument(char** argv, size_t token_offset, float& v)
{
  const char* startptr = *argv + token_offset;
  char* argend = *argv + std::strlen(*argv);

  if (startptr == argend)
  {
    startptr = *++argv;
    argend = *argv + std::strlen(*argv);
  }

  char* endptr = nullptr;
  v = std::strtof(startptr, &endptr);

  if (endptr < argend)
    throw usage_error("expected float argument");

  return argv;
}

char** parseArgument(char** argv, size_t token_offset, std::string& v)
{
  const char* startptr = *argv + token_offset;
  char* argend = *argv + std::strlen(*argv);

  if (startptr == argend)
  {
    startptr = *++argv;
    argend = *argv + std::strlen(*argv);
  }

  char* endptr = nullptr;
  v = std::string(startptr);

  //if (endptr < argend)
    //throw usage_error("expected string argument");

  return argv;
}

template <int S, typename T1, typename T2>
bool checkArgument(const char (&token)[S], char**& a, T1& v1, T2& v2)
{
  if (std::strncmp(token, *a, strlen(token)) == 0)
  {
    a = parseArgument(++a, 0, v1);
    a = parseArgument(++a, 0, v2);
    return true;
  }
  return false;
}
template <int S, typename T>
bool checkArgument(const char (&token)[S], char**& a, T& v)
{
  if (std::strncmp(token, *a, strlen(token)) == 0)
  {
    a = parseArgument(a, strlen(token), v);
    return true;
  }
  return false;
}
template <int S>
bool checkArgument(const char (&token)[S], char**& a, bool& set)
{
  if (std::strncmp(token, *a, strlen(token)) == 0)
  {
    set = true;
    return true;
  }
  return false;
}
}
