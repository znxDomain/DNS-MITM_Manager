#include <sstream>
#include <vector>
#include <tesla.hpp>

namespace GlobalObjects {

  static std::string _version_str_ = "v0.0.2";

  static std::string _hosts_file_ = "";

  void set_hosts_file(const std::basic_string<char>& hosts_file);
  
  static std::vector<std::string> _hosts_entries_list_;
  
  std::vector<std::string> & get_hosts_entries_list();
  void fill_hosts_entries_list();
  void clear_hosts_entries_list();

  void toggle_entry(int index, bool state);
  void save_file();

  void LogLine(const char* fmt, ...);

};
