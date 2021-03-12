#include <tesla.hpp>


class FileBrowserGui : public tsl::Gui {
public:

  FileBrowserGui();

  // Called when this Gui gets loaded to create the UI
  // Allocate all elements on the heap. libtesla will make sure to clean them up when not needed anymore
  tsl::elm::Element* createUI() override;

  void fill_item_list();

  // Called once every frame to update values
  // void update() override;

private:

  tsl::elm::OverlayFrame* _frame_;
  tsl::elm::List* _list_;

};
