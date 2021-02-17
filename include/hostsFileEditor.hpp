#pragma once
#include <tesla.hpp>

#include <list>

struct HostsEntry {
    tsl::elm::Element *listItem;
    int lineNum;
    std::string raw;
};

class HostsFileEditor : public tsl::Gui {
  private:
    FsFileSystem he_fs;
    std::list<HostsEntry> he_fileEditorListItems;
    bool he_scanned;
    void saveFile();

  public:
    HostsFileEditor(const char* fileName);
    ~HostsFileEditor();

    virtual tsl::elm::Element *createUI();

  private:
};
