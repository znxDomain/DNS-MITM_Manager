#include "hostsFileSelector.hpp"
#include "hostsFileEditor.hpp"
#include "dir_iterator.hpp"

static char pathBuffer2[FS_MAX_PATH];

HostsFileSelector::HostsFileSelector() {
    Result rc = fsOpenSdCardFileSystem(&this->hf_fs);
    if (R_FAILED(rc))
        return;

    FsDir hostsDir;
    std::strcpy(pathBuffer2, amsHostsPath);
    rc = fsFsOpenDirectory(&this->hf_fs, pathBuffer2, FsDirOpenMode_ReadFiles, &hostsDir);
    if (R_FAILED(rc))
        return;
    tsl::hlp::ScopeGuard dirGuard([&] { fsDirClose(&hostsDir); });

    /* Iterate over contents folder. */
    for (const auto &entry : FsDirIterator(hostsDir)) {       
        HostsFile hFile = {
            .listItem = new tsl::elm::ListItem(entry.name),
            .fileName = entry.name,
        };

        hFile.listItem->setClickListener([this, hFile, entry](u64 click) -> bool {
            if (click & HidNpadButton_A) {
                tsl::changeTo<HostsFileEditor>(entry.name);
                return true;
            }
            return false;
        });
        this->hf_hostsFileListItems.push_back(std::move(hFile));
    }
    this->hf_scanned = true;
}

HostsFileSelector::~HostsFileSelector() {
    fsFsClose(&this->hf_fs);
}

tsl::elm::Element *HostsFileSelector::createUI() {
    tsl::elm::OverlayFrame *rootFrame = new tsl::elm::OverlayFrame("Hosts Editor/DNS-MITM Reload", "v0.0.2");

    if (this->hf_hostsFileListItems.size() == 0) {
        const char *description = this->hf_scanned ? "No hosts files found!" : "Directory scan failed!";

        auto *warning = new tsl::elm::CustomDrawer([description](tsl::gfx::Renderer *renderer, s32 x, s32 y, s32 w, s32 h) {
            renderer->drawString("\uE150", false, 180, 250, 90, renderer->a(0xFFFF));
            renderer->drawString(description, false, 110, 340, 25, renderer->a(0xFFFF));
        });

        rootFrame->setContent(warning);
    } else {
        tsl::elm::List *hostsFileList = new tsl::elm::List();
        hostsFileList->addItem(new tsl::elm::CategoryHeader("Choose a file to edit:"));
        
        for (const auto &hFile : this->hf_hostsFileListItems) {
            hostsFileList->addItem(hFile.listItem);
        }
        rootFrame->setContent(hostsFileList);
    }

    return rootFrame;
}
