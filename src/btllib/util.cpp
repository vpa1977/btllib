#include "btllib/util.hpp"
#include "btllib/cstring.hpp"

#include <algorithm>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>

namespace btllib {

std::vector<std::string>
split(const std::string& s, const std::string& delim)
{
  std::vector<std::string> tokens;
  size_t pos1 = 0, pos2 = 0;
  while ((pos2 = s.find(delim, pos2)) != std::string::npos) {
    tokens.push_back(s.substr(pos1, pos2 - pos1));
    pos2 += delim.size();
    pos1 = pos2;
  }
  tokens.push_back(s.substr(pos1));
  return tokens;
}

std::string
join(const std::vector<std::string>& s, const std::string& delim)
{
  std::string joined = s[0];
  for (size_t i = 1; i < s.size(); i++) {
    joined += delim;
    joined += s[i];
  }
  return joined;
}

void
ltrim(std::string& s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !bool(std::isspace(ch));
          }));
}

void
ltrim(btllib::CString& s)
{
  decltype(s.size()) i = 0;
  while (i < s.size() && bool(std::isspace(s[i]))) {
    i++;
  }
  s.erase(0, i);
}

void
rtrim(std::string& s)
{
  s.erase(std::find_if(s.rbegin(),
                       s.rend(),
                       [](int ch) { return !bool(std::isspace(ch)); })
            .base(),
          s.end());
}

void
rtrim(btllib::CString& s)
{
  auto i = s.size();
  while (i > 0 && bool(std::isspace(s[i - 1]))) {
    i--;
  }
  s.resize(i);
}

void
trim(std::string& s)
{
  ltrim(s);
  rtrim(s);
}

void
trim(btllib::CString& s)
{
  ltrim(s);
  rtrim(s);
}

bool
startswith(std::string s, std::string prefix)
{
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
  std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);
  return s.find(prefix) == 0;
}

bool
endswith(std::string s, std::string suffix)
{
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
  std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);
  auto pos = s.rfind(suffix);
  return (pos != std::string::npos) && (pos == s.size() - suffix.size());
}

std::string
get_dirname(const std::string& path)
{
  std::string ret = path;
  auto last_slash_pos = path.find_last_of('/');

  if (last_slash_pos != std::string::npos && last_slash_pos != 0 &&
      last_slash_pos == path.size() - 1) {
    auto i = last_slash_pos;
    for (; i != 0; --i) {
      if (path[i - 1] != '/') {
        break;
      }
    }
    if (i != 0) {
      last_slash_pos = path.substr(0, i).find_last_of('/');
    }
  }

  if (last_slash_pos != std::string::npos) {
    auto i = last_slash_pos;
    for (; i != 0; --i) {
      if (path[i - 1] != '/') {
        break;
      }
    }
    if (i == 0) {
      if (last_slash_pos == 1) {
        ++last_slash_pos;
      } else {
        last_slash_pos = 1;
      }
    } else {
      last_slash_pos = i;
    }
    ret.resize(last_slash_pos);
  } else {
    return ".";
  }

  return ret;
}

std::string
get_basename(const std::string& path)
{
  const auto p = path.find_last_of('/');
  if (p == std::string::npos) {
    return path;
  }
  return path.substr(p + 1);
}

void
Barrier::wait()
{
  std::unique_lock<std::mutex> lock(m);
  ++counter;
  ++waiting;
  cv.wait(lock, [&] { return counter >= counter_default; });
  cv.notify_one();
  --waiting;
  if (waiting == 0) {
    counter = 0;
  }
}

} // namespace btllib