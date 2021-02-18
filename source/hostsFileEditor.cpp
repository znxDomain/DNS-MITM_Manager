#include "hostsFileEditor.hpp"
#include <sstream>      // std::stringstream, std::stringbuf
#include <iostream>
#include <string>
#include <fstream>

constexpr const char *const amsHostsPath = "/atmosphere/hosts";
static char pathBuffer[FS_MAX_PATH];

std::list<HostsEntry> he_fileEditorListItems;

/*
-on file writes:
  -create a new temp file with contents
  -rename original file
  -rename temp file
  -delete original file
*/

int writes = 0;
const char* g_arg1;
bool saveFile(u64 keys) {
    FsFileSystem sf_fs;
    std::string hostsData;
    hostsData += std::__cxx11::to_string(writes);
    hostsData += "\n";
    hostsData += "TEST\n";

    if (he_fileEditorListItems.size() != 0) {
        for (const auto &hEntry : he_fileEditorListItems) {
            if(hEntry.listItem->getState()){
                hostsData += hEntry.listItem->getText();
            } else {
                hostsData += ";";
                hostsData += hEntry.listItem->getText();
            }
        }
    }

    Result rc = fsOpenSdCardFileSystem(&sf_fs);
    if (R_FAILED(rc))
        return true;

    // Write Changes to file
    fsFsDeleteFile(&sf_fs, "/atmosphere/hosts/temp-editor.txt ");
    fsFsCreateFile(&sf_fs, "/atmosphere/hosts/temp-editor.txt", 0, 0);        

    FsFile hostsFile;
    rc = fsFsOpenFile(&sf_fs, "/atmosphere/hosts/temp-editor.txt", (FsOpenMode_Read | FsOpenMode_Write | FsOpenMode_Append), &hostsFile);
    // tsl::hlp::ScopeGuard fileGuard([&] { fsFileClose(&hostsFile); });
    rc = fsFileWrite(&hostsFile, 0, hostsData.data(), hostsData.length(), FsWriteOption_None);
    fsFileClose(&hostsFile);

    // Backup original
    fsFsRenameFile(&sf_fs, "/atmosphere/hosts/editor.txt", "/atmosphere/hosts/editor.bak") ;

    // Move new file into original place
    fsFsRenameFile(&sf_fs, "/atmosphere/hosts/temp-editor.txt", "/atmosphere/hosts/editor.txt") ;

    // Delete backup
    fsFsDeleteFile(&sf_fs, "/atmosphere/hosts/editor.bak");

    writes += 1;

    return false;
}


HostsFileEditor::HostsFileEditor(const char* fileName) {
    g_arg1 = fileName;
    Result rc = fsOpenSdCardFileSystem(&this->he_fs);
    if (R_FAILED(rc))
        return;

    FsFile hostsFile;
    std::snprintf(pathBuffer, FS_MAX_PATH, "/atmosphere/hosts/%s", "sysmmc.txt");
    rc = fsFsOpenFile(&this->he_fs, pathBuffer, FsOpenMode_Read, &hostsFile);
    if (R_FAILED(rc))
        return;
    tsl::hlp::ScopeGuard fileGuard([&] { fsFileClose(&hostsFile); });

    /* Get hosts file size. */
    s64 size;
    rc = fsFileGetSize(&hostsFile, &size);
    if (R_FAILED(rc))
        return;

    /* Read hosts file. */
    std::string hostsData(size, '\0');
    u64 bytesRead;
    rc = fsFileRead(&hostsFile, 0, hostsData.data(), size, FsReadOption_None, &bytesRead);
    if (R_FAILED(rc))
        return;

    std::stringstream ss;
    ss.str(hostsData);

    std::string line = "";
    int count = 0;
    while (std::getline(ss, line)) {
        // Section Header:
        // - lines that start with a #
        // Entries:
        // - lines that start with a digit
        // - lines that start with a ;
        // Stuff that won't be displayed, and can't be changed:
        // - everything else
        tsl::elm::ToggleListItem *item = NULL;
        tsl::elm::CategoryHeader *item2 = NULL;
        if (line.size() > 1){
            switch (line[0]){
                case '0'...'9':
                    item = new tsl::elm::ToggleListItem(line, true);
                    item->setClickListener(saveFile);
                    break;
                case ';':
                    item = new tsl::elm::ToggleListItem(line.substr(1, line.size() - 1), false);
                    item->setClickListener(saveFile);
                    break;
                case '#':
                    item2 = new tsl::elm::CategoryHeader(line.substr(1, line.size() - 1));
                    break;  
            }
        }
        HostsEntry hEntry = {
            .listItem = item,
            .headerItem = item2,
            .lineNum = count,
            .raw = line,
        };

        // hEntry.listItem->setClickListener()
        he_fileEditorListItems.push_back(std::move(hEntry));
        count += 1;
    }
    this->he_scanned = true;
}

HostsFileEditor::~HostsFileEditor() {
    fsFsClose(&this->he_fs);
}


tsl::elm::Element *HostsFileEditor::createUI() {
    tsl::elm::OverlayFrame *rootFrame = new tsl::elm::OverlayFrame("Toggle Hosts Entries:",g_arg1);

    if (he_fileEditorListItems.size() == 0) {
        const char *description = this->he_scanned ? "No entries found in file!" : "File scan failed!";

        auto *warning = new tsl::elm::CustomDrawer([description](tsl::gfx::Renderer *renderer, s32 x, s32 y, s32 w, s32 h) {
            renderer->drawString("\uE150", false, 180, 250, 90, renderer->a(0xFFFF));
            renderer->drawString(description, false, 110, 340, 25, renderer->a(0xFFFF));
        });

        rootFrame->setContent(warning);
    } else {
        tsl::elm::List *hostsEntryList = new tsl::elm::List();   

        for (const auto &hEntry : he_fileEditorListItems) {
                hostsEntryList->addItem(hEntry.headerItem);
                hostsEntryList->addItem(hEntry.listItem);
        }
        rootFrame->setContent(hostsEntryList);
    }

    return rootFrame;
}
