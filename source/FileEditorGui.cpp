#include <FileEditorGui.h>
#include <GlobalObjects.h>


FileEditorGui::FileEditorGui(const std::basic_string<char> &current_sub_folder_) {
  _current_sub_folder_ = current_sub_folder_;
  _frame_ = nullptr;
  _list_ = nullptr;
  _trigger_item_list_update_ = false;
}

tsl::elm::Element *FileEditorGui::createUI() {

  _frame_ = new tsl::elm::OverlayFrame("DNS-MITM Manager", _current_sub_folder_);
  _list_ = new tsl::elm::List();

  GlobalObjects::set_hosts_file(_current_sub_folder_);

  // A list that can contain sub elements and handles scrolling
  fill_item_list();

  return _frame_;

}

void FileEditorGui::fill_item_list() {
  // List Items
  GlobalObjects::clear_hosts_entries_list();
  GlobalObjects::fill_hosts_entries_list();
  auto entries_list = GlobalObjects::get_hosts_entries_list();

  for (int i_entry = 0; i_entry < int(entries_list.size()); i_entry++) {
    switch (entries_list[i_entry][0]){
      // IP Address
      case '0'...'9':
      {
        auto *clickableListItem = new tsl::elm::ToggleListItem(entries_list[i_entry], true);

        clickableListItem->setClickListener([i_entry, clickableListItem](u64 keys) {
          if (keys & HidNpadButton_A) {
            // toggle entry...
            GlobalObjects::toggle_entry(i_entry, clickableListItem->getState());

            return true;
          }
          return false;
        });

        _list_->addItem(clickableListItem);

        break;
      }
      // Commented IP address
      case ';':
      {
        auto *clickableListItem = new tsl::elm::ToggleListItem(entries_list[i_entry].substr(1, entries_list[i_entry].size() - 1), false);

        clickableListItem->setClickListener([i_entry, clickableListItem](u64 keys) {
          if (keys & HidNpadButton_A) {
            // toggle entry...
            GlobalObjects::toggle_entry(i_entry, clickableListItem->getState());
            return true;
          }
          return false;
        });
        _list_->addItem(clickableListItem);

        break;
      }
      // Header
      case '#':
      {
        auto *clickableListItem = new tsl::elm::CategoryHeader(entries_list[i_entry].substr(1, entries_list[i_entry].size() - 1));
        _list_->addItem(clickableListItem);
        break; 
      }
    }
  }

  // Add the list to the frame for it to be drawn
  _frame_->setContent(_list_); // will delete previous list

}

