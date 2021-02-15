#define TESLA_INIT_IMPL
#include "hostsFileSelector.hpp"

class MainOverlay : public tsl::Overlay {
  public:
    MainOverlay() {}
    ~MainOverlay() {}
                                           // libtesla already initialized fs, hid, pl, pmdmnt, hid:sys and set:sys
    virtual void initServices() override {}  // Called at the start to initialize all services necessary for this Overlay
    virtual void exitServices() override {}  // Callet at the end to clean up all services previously initialized

    virtual void onShow() override {}    // Called before overlay wants to change from invisible to visible state
    virtual void onHide() override {     // Called before overlay wants to change from visible to invisible state
        Result rc1;
        Result rc2;
        static Service sfdnsresSrv;

        // Call sfdnsres service to reload hosts file
        tsl::hlp::doWithSmSession([&]{
            rc1 = smGetService(&sfdnsresSrv, "sfdnsres");
            rc2 = serviceDispatch(&sfdnsresSrv, 65000);
            serviceClose(&sfdnsresSrv);
        });
    }

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<HostsFileSelector>();  // Initial Gui to load.
    }
};

int main(int argc, char **argv) {
    return tsl::loop<MainOverlay>(argc, argv);
}