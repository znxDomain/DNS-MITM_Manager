#pragma once
#include <tesla.hpp>

#include <list>

struct HostsFile {
    tsl::elm::ListItem *listItem;
    const char* fileName;
};

class HostsFileSelector : public tsl::Gui {
  private:
    FsFileSystem hf_fs;
    std::list<HostsFile> hf_hostsFileListItems;
    bool hf_scanned;

  public:
    HostsFileSelector();
    ~HostsFileSelector();

    virtual tsl::elm::Element *createUI();

  private:
};
