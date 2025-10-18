#include "fileDialog.hpp"
#ifdef _WIN32
    #include <windows.h>
    #include <ShObjIdl.h>

std::vector<std::filesystem::path> FileDialog::ShowFileDialog(bool isSave)
{
    IFileOpenDialog* pFileOpen = nullptr;
    bool bIsAllOperationSurcces = false;
    bool bSelectFolder = false;
    bool bMultiSelect = false;
    std::vector<std::filesystem::path> paths;
    while (!bIsAllOperationSurcces)
    {
        CLSID dialogType = CLSID_FileOpenDialog;
        if (isSave)
            dialogType = CLSID_FileSaveDialog;
        HRESULT hr = CoCreateInstance(dialogType, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void **>(&pFileOpen));
        if (FAILED(hr))
            break;
        const COMDLG_FILTERSPEC rgSpec[] =
        {
            { L"All Supported Files (*.stp, *.step,*.glb, *.glTF, *.obj, *.fbx)", L"*.glb; *.glTF; *.obj; *.fbx"},
            { L"STEP Files (*.stp, *.step)", L"*.stp; *.step"},
            { L"GLTF Files (*.glb, *.glTF)", L"*.glb; *.glTF"},
            { L"OBJ Files (*.obj)", L"*.obj"},
            { L"FBX Files (*.fbx)", L"*.fbx"}
        };
        pFileOpen->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);
        if (bSelectFolder || bMultiSelect)
        {
            FILEOPENDIALOGOPTIONS options = 0;
            hr = pFileOpen->GetOptions(&options);
            if(FAILED(hr))
                break;
            if (bSelectFolder)
                options |= FOS_PICKFOLDERS;
            if (bMultiSelect)
                options |= FOS_ALLOWMULTISELECT;
            hr = pFileOpen->SetOptions(options);
            if (FAILED(hr))
                break;
        }
        hr = pFileOpen->Show(nullptr);
        if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
        {
            bIsAllOperationSurcces = true;
            break;
        }
        else if (FAILED(hr))
            break;
        IShellItemArray* pItems;
        hr = pFileOpen->GetResults(&pItems);
        if (FAILED(hr))
            break;
        DWORD totalItems = 0;
        hr = pItems->GetCount(&totalItems);
        if (FAILED(hr))
            break;
        for (int i=0 ; i<static_cast<int>(totalItems); ++i)
        {
            IShellItem* pItem;
            pItems->GetItemAt(i, &pItem);
            if (SUCCEEDED(hr))
            {
                PWSTR path;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &path);
                if (SUCCEEDED(hr))
                {
                    paths.push_back(std::filesystem::path(path));
                    CoTaskMemFree(path);
                }
                pItem->Release();
            }
        }
        pItems->Release();
        bIsAllOperationSurcces = true;
    }
    if (pFileOpen)
        pFileOpen->Release();
    return paths;
}
#endif