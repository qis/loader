// ユニコード
#include <os/process.h>

int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE, PWSTR cmd, int show) {
  try {
    os::process process;
    for (const auto& pe : os::process_snapshot(TH32CS_SNAPPROCESS, 0)) {
      if (std::wstring_view(pe.szExeFile) == L"gvim.exe") {
        process = os::process(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pe.th32ProcessID);
        break;
      }
    }
    if (!process) {
      throw std::runtime_error("Process not found.");
    }
  }
  catch (const std::exception& e) {
    std::wstring what;
    what.resize(MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, nullptr, 0) + 1);
    what.resize(MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, what.data(), static_cast<int>(what.size())));
    MessageBox(nullptr, what.data(), L"Error", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
    return 1;
  }
  return 0;
}