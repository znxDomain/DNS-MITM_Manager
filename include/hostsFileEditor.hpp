#pragma once
#include <tesla.hpp>

#include <list>
constexpr const char *const amsHostsPath = "/atmosphere/hosts";
static char pathBuffer[FS_MAX_PATH];

struct HostsEntry {
    tsl::elm::ToggleListItem *listItem ;
    tsl::elm::CategoryHeader *categoryItem;
    int lineNum;
    std::string raw;
};

class HostsFileEditor : public tsl::Gui {
  private:
    FsFileSystem he_fs;
    std::list<HostsEntry> he_fileEditorListItems;
    bool he_scanned;
    std::function<bool (u64)> saveFile = [*this] (u64 keys) {
      std::string hostsData;
      hostsData += "\n";
      if (this->he_fileEditorListItems.size() != 0) {
          for (const auto &hEntry : this->he_fileEditorListItems) {
              if(hEntry.listItem->getState()){
                  hostsData += hEntry.listItem->getText();
              } else {
                  hostsData += ";";
                  hostsData += hEntry.listItem->getText();
              }
          }
      }
      Result rc = fsOpenSdCardFileSystem((FsFileSystem*) &this->he_fs);
      if (R_FAILED(rc))
          return true;

      FsFile hostsFile;
      std::snprintf(pathBuffer, FS_MAX_PATH, "/atmosphere/hosts/%s","sysmmc.txt");
      rc = fsFsOpenFile((FsFileSystem*) &this->he_fs, pathBuffer, FsOpenMode_Write, &hostsFile);
      if (R_FAILED(rc))
          return true;
      tsl::hlp::ScopeGuard fileGuard([&] { fsFileClose(&hostsFile); });

      /* Write hosts file. */
      rc = fsFileWrite(&hostsFile, 0, hostsData.data(), hostsData.size(), FsWriteOption_Flush);
      if (R_FAILED(rc))
          return true;

      return false;
  };

  public:
    HostsFileEditor(const char* fileName);
    ~HostsFileEditor();

    virtual tsl::elm::Element *createUI();

  private:
};
