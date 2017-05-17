// ユニコード
#pragma once

namespace os {

class process {
public:
  process() noexcept = default;

  process(DWORD access, BOOL inherit_handle, DWORD id) {
    open(access, inherit_handle, id);
  }

  process(process&& other) noexcept : handle_(std::exchange(other.handle_, invalid_handle_value())) {
  }

  process& operator=(process&& other) noexcept {
    close();
    handle_ = std::exchange(other.handle_, invalid_handle_value());
    return *this;
  }

  ~process() {
    close();
  }

  operator bool() const noexcept {
    return handle_ != invalid_handle_value();
  }

  void open(DWORD access, BOOL inherit_handle, DWORD id) {
    std::error_code ec;
    open(access, inherit_handle, id, ec);
    if (ec) {
      throw std::system_error(ec, "Could not open process");
    }
  }

  void open(DWORD access, BOOL inherit_handle, DWORD id, std::error_code& ec) noexcept {
    close();
    handle_ = OpenProcess(access, inherit_handle, id);
    ec = std::error_code(GetLastError(), std::system_category());
  }

  void close() noexcept {
    const auto handle = std::exchange(handle_, invalid_handle_value());
    if (handle != invalid_handle_value()) {
      CloseHandle(handle);
    }
  }

  static constexpr HANDLE invalid_handle_value() noexcept {
    return nullptr;
  }

private:
  HANDLE handle_ = invalid_handle_value();
};

class process_snapshot {
public:
  class iterator {
  public:
    using value_type = PROCESSENTRY32;
    using iterator_category = std::input_iterator_tag;
    using pointer = const value_type*;
    using reference = const value_type&;

    iterator() noexcept = default;

    explicit iterator(process_snapshot* snapshot) noexcept : snapshot_(snapshot) {
    }

    reference operator*() const noexcept {
      return snapshot_->entry_;
    }

    iterator& operator++() noexcept {
      if (!snapshot_->next()) {
        snapshot_ = nullptr;
      }
      return *this;
    }

    iterator operator++(int) noexcept {
      return ++iterator(*this);
    }

    bool operator==(const iterator& rhs) noexcept {
      return snapshot_ == rhs.snapshot_;
    }

    bool operator!=(const iterator& rhs) noexcept {
      return snapshot_ != rhs.snapshot_;
    }

  private:
    process_snapshot* snapshot_ = nullptr;
  };

  process_snapshot() noexcept {
    entry_.dwSize = sizeof(entry_);
  }

  process_snapshot(DWORD flags, DWORD id) : process_snapshot() {
    open(flags, id);
  }

  process_snapshot(process_snapshot&& other) noexcept : handle_(std::exchange(other.handle_, invalid_handle_value())), entry_(other.entry_) {
  }

  process_snapshot& operator=(process_snapshot&& other) noexcept {
    close();
    handle_ = std::exchange(other.handle_, invalid_handle_value());
    entry_ = other.entry_;
    return *this;
  }

  ~process_snapshot() {
    close();
  }

  operator bool() const noexcept {
    return handle_ != invalid_handle_value();
  }

  void open(DWORD flags, DWORD id) {
    std::error_code ec;
    open(flags, id, ec);
    if (ec) {
      throw std::system_error(ec, "Could not create process snapshot");
    }
  }

  void open(DWORD flags, DWORD id, std::error_code& ec) noexcept {
    close();
    handle_ = CreateToolhelp32Snapshot(flags, id);
    ec = std::error_code(GetLastError(), std::system_category());
  }

  void close() noexcept {
    const auto handle = std::exchange(handle_, invalid_handle_value());
    if (handle != invalid_handle_value()) {
      CloseHandle(handle);
    }
  }

  iterator begin() noexcept {
    return first() ? iterator(this) : iterator();
  }

  iterator end() noexcept {
    return {};
  }

  constexpr static HANDLE invalid_handle_value() noexcept {
    return INVALID_HANDLE_VALUE;
  }

private:
  BOOL first() noexcept {
    return Process32First(handle_, &entry_);
  }

  BOOL next() noexcept {
    return Process32Next(handle_, &entry_);
  }

  HANDLE handle_ = invalid_handle_value();
  PROCESSENTRY32 entry_ = {};
};

}  // namespace os