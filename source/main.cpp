#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#include <tesla.hpp>    // The Tesla Header

#include <GlobalObjects.h>
#include <FileBrowserGui.h>


class DnsMitmManagerOverlay : public tsl::Overlay {
public:

  // libtesla already initialized fs, hid, pl, pmdmnt, hid:sys and set:sys
  void initServices() override {

  }  // Called at the start

  void exitServices() override {

  }  // Callet at the end to clean up all services previously initialized

  void onShow() override {}    // Called before overlay wants to change from invisible to visible state
  void onHide() override {     // Called before overlay wants to change from visible to invisible state
    static Service sfdnsresSrv;

    // Call sfdnsres service to reload hosts file
    tsl::hlp::doWithSmSession([&]{
      smGetService(&sfdnsresSrv, "sfdnsres");
      serviceDispatch(&sfdnsresSrv, 65000);
      serviceClose(&sfdnsresSrv);
    });
  }

  std::unique_ptr<tsl::Gui> loadInitialGui() override {
    return initially<FileBrowserGui>();  // Initial Gui to load. It's possible to pass arguments to it's constructor like this
  }
};


int main(int argc, char **argv) {
  return tsl::loop<DnsMitmManagerOverlay>(argc, argv);
}
