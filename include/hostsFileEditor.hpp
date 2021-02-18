#pragma once
#include <tesla.hpp>

#include <list>

bool saveFile(u64 keys);

struct HostsEntry {
    tsl::elm::ToggleListItem *listItem;
    tsl::elm::CategoryHeader *headerItem;
    int lineNum;
    std::string raw;
};

class HostsFileEditor : public tsl::Gui {
  private:
    FsFileSystem he_fs;
    bool he_scanned;

  public:
    HostsFileEditor(const char* fileName);
    ~HostsFileEditor();

    virtual tsl::elm::Element *createUI();

  private:
};
