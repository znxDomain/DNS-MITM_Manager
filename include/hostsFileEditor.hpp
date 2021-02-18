#pragma once
#include <tesla.hpp>

#include <list>

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
    bool saveFile(u64 keys);

  public:
    HostsFileEditor(const char* fileName);
    ~HostsFileEditor();

    virtual tsl::elm::Element *createUI();

  private:
};
