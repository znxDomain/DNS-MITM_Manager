#include "GlobalObjects.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <cerrno>

#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

#define ENABLE_LOGGING 0

namespace GlobalObjects{

  void LogLine(const char* fmt, ...)
  {
  #ifdef ENABLE_LOGGING
    stdout = stderr = fopen("/error.log", "a");
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    fclose(stdout);
  #endif
  }

  void set_hosts_file(const std::basic_string<char>& hosts_file){
    GlobalObjects::_hosts_file_ = hosts_file;
  }

  std::vector<std::string> & get_hosts_entries_list(){
    return _hosts_entries_list_;
  }

  void fill_hosts_entries_list(){
    FsFileSystem fsSdmc;
    if (R_FAILED(fsOpenSdCardFileSystem(&fsSdmc)))
        LogLine("Error fsOpenSdCardFileSystem\n");
    tsl::hlp::ScopeGuard fsGuard([&] { fsFsClose(&fsSdmc); });

    FsFile hostsFile;
    static char pathBuffer[FS_MAX_PATH];
    std::snprintf(pathBuffer, FS_MAX_PATH, "/atmosphere/hosts/%s", GlobalObjects::_hosts_file_.c_str());
    if (R_FAILED(fsFsOpenFile(&fsSdmc, pathBuffer, FsOpenMode_Read, &hostsFile)))
        LogLine("Error fsFsOpenFile");
    tsl::hlp::ScopeGuard fileGuard([&] { fsFileClose(&hostsFile); });

    /* Get hosts file size. */
    s64 size;
    if (R_FAILED(fsFileGetSize(&hostsFile, &size)))
        LogLine("Error fsFileGetSize");

    /* Read hosts file. */
    std::string hostsData(size, '\0');
    u64 bytesRead;
    if (R_FAILED(fsFileRead(&hostsFile, 0, hostsData.data(), size, FsReadOption_None, &bytesRead)))
        LogLine("Error fsFileRead");

    std::stringstream ss;
    ss.str(hostsData);

    std::string line;
    while (std::getline(ss, line)) {
      _hosts_entries_list_.push_back(line);
    }
  }

  void clear_hosts_entries_list(){
    _hosts_entries_list_.clear();
  }

  void toggle_entry(int index, bool state){
    // LogLine("toggle_entry %X\n", state);
    if (state){
      _hosts_entries_list_[index].erase(0, 1);
    } else {
      _hosts_entries_list_[index].insert(0, 1, ';');
    }
    GlobalObjects::save_file();
    return;
  }

  void save_file(){    
    FsFileSystem fsSdmc;
    Result rc = fsOpenSdCardFileSystem(&fsSdmc);
    if (R_FAILED(rc))
        fatalThrow(rc);
    tsl::hlp::ScopeGuard fsGuard([&] { fsFsClose(&fsSdmc); });

    FsFile hostsFile;
    static char originalFile[FS_MAX_PATH];
    static char backupFile[FS_MAX_PATH];
    static char tempFile[FS_MAX_PATH];

    std::snprintf(originalFile, FS_MAX_PATH, "/atmosphere/hosts/%s", GlobalObjects::_hosts_file_.c_str());
    std::snprintf(backupFile, FS_MAX_PATH, "/atmosphere/hosts/%s.bak", GlobalObjects::_hosts_file_.c_str());
    std::snprintf(tempFile, FS_MAX_PATH, "/atmosphere/hosts/%s.tmp", GlobalObjects::_hosts_file_.c_str());

    // Write Changes to a temporary file
    rc = fsFsDeleteFile(&fsSdmc, tempFile);
    // if (R_FAILED(rc))
    //     fatalThrow(rc);

    rc = fsFsCreateFile(&fsSdmc, tempFile, 0, 0);   
    if (R_FAILED(rc))
        fatalThrow(rc);

    rc = fsFsOpenFile(&fsSdmc, tempFile, (FsOpenMode_Read | FsOpenMode_Write | FsOpenMode_Append), &hostsFile);
    if (R_FAILED(rc))
        fatalThrow(rc);
    tsl::hlp::ScopeGuard fileGuard([&] { fsFileClose(&hostsFile); });

    std::string hostsData;
    
    for (u32 i = 0; i < _hosts_entries_list_.size(); i++) {
      hostsData += _hosts_entries_list_[i];
      hostsData += "\n";
    }

    rc = fsFileWrite(&hostsFile, 0, hostsData.data(), hostsData.length(), FsWriteOption_None);
    if (R_FAILED(rc))
        fatalThrow(rc);

    fsFileClose(&hostsFile);

    // Backup original
    rc = fsFsRenameFile(&fsSdmc, originalFile, backupFile);
    if (R_FAILED(rc))
        fatalThrow(rc);

    // Move new file into original place
    rc = fsFsRenameFile(&fsSdmc, tempFile, originalFile) ;
    if (R_FAILED(rc))
        fatalThrow(rc);

    // Delete backup
    rc = fsFsDeleteFile(&fsSdmc, backupFile);
    if (R_FAILED(rc))
        fatalThrow(rc);

    return;
  }

}
