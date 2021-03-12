#include "FileBrowserGui.h"
#include "dir_iterator.hpp"

#include <GlobalObjects.h>
#include <FileEditorGui.h>

FileBrowserGui::FileBrowserGui() {
  _frame_ = nullptr;
  _list_ = nullptr;
}

tsl::elm::Element *FileBrowserGui::createUI() {
// A OverlayFrame is the base element every overlay consists of. This will draw the default Title and Subtitle.
// If you need more information in the header or want to change it's look, use a HeaderOverlayFrame.
  _frame_ = new tsl::elm::OverlayFrame("DNS-MITM Manager", "v0.0.2");

// A list that can contain sub elements and handles scrolling
  _list_ = new tsl::elm::List();

  fill_item_list();

// Return the frame to have it become the top level element of this Gui
  return _frame_;
}

void FileBrowserGui::fill_item_list() {

  _list_->clear();

  // List Items
  _list_->addItem(new tsl::elm::CategoryHeader("Choose a file to edit : "));
  
  FsFileSystem fsSdmc;
  if (R_FAILED(fsOpenSdCardFileSystem(&fsSdmc)))
      GlobalObjects::LogLine("Error fsOpenSdCardFileSystem\n");
  tsl::hlp::ScopeGuard fsGuard([&] { fsFsClose(&fsSdmc); });

  FsDir hostsDir;
  static char pathBuffer[FS_MAX_PATH];
  std::strcpy(pathBuffer, "/atmosphere/hosts");

  if (R_FAILED(fsFsOpenDirectory(&fsSdmc, pathBuffer, FsDirOpenMode_ReadFiles, &hostsDir)))
      return;
  tsl::hlp::ScopeGuard dirGuard([&] { fsDirClose(&hostsDir); });

  for (const auto &entry : FsDirIterator(hostsDir)) {
    auto *clickableListItem = new tsl::elm::ListItem(entry.name);
    std::string selected_file = entry.name;
    clickableListItem->setClickListener([selected_file](u64 keys) {
      if (keys & HidNpadButton_A) {
        tsl::changeTo<FileEditorGui>(selected_file);
        return true;
      }
      return false;
    });
    _list_->addItem(clickableListItem);

  }

  // Add the list to the frame for it to be drawn
  _frame_->setContent(_list_);

}
