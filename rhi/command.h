//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <utility>
#include "map_macro.h"

namespace vox {
class Buffer;

#define COMPUTE_RUNTIME_COMMANDS    \
    BufferUploadCommand,            \
        BufferDownloadCommand,      \
        BufferCopyCommand,          \
        BufferToTextureCopyCommand, \
        CustomCommand

#define MAKE_COMMAND_FWD_DECL(CMD) class CMD;
MAP(MAKE_COMMAND_FWD_DECL, COMPUTE_RUNTIME_COMMANDS)
#undef MAKE_COMMAND_FWD_DECL

struct CommandVisitor {
#define MAKE_COMMAND_VISITOR_INTERFACE(CMD) \
    virtual void visit(const CMD *) noexcept = 0;
    MAP(MAKE_COMMAND_VISITOR_INTERFACE, COMPUTE_RUNTIME_COMMANDS)
#undef MAKE_COMMAND_VISITOR_INTERFACE
    virtual ~CommandVisitor() noexcept = default;
};

class Command {
public:
    Command() noexcept = default;
    virtual ~Command() noexcept = default;
    virtual void accept(CommandVisitor &visitor) const noexcept = 0;
};

class BufferUploadCommand final : public Command {
private:
    std::shared_ptr<const Buffer> _handle{};
    size_t _offset{};
    size_t _size{};
    const void *_data{};

public:
    BufferUploadCommand(std::shared_ptr<const Buffer> handle,
                        size_t offset_bytes,
                        size_t size_bytes,
                        const void *data) noexcept
        : Command{},
          _handle{std::move(handle)}, _offset{offset_bytes}, _size{size_bytes}, _data{data} {}
    [[nodiscard]] auto handle() const noexcept { return _handle; }
    [[nodiscard]] auto offset() const noexcept { return _offset; }
    [[nodiscard]] auto size() const noexcept { return _size; }
    [[nodiscard]] auto data() const noexcept { return _data; }
    void accept(CommandVisitor &visitor) const noexcept override { visitor.visit(this); }
};

class BufferDownloadCommand final : public Command {
private:
    std::shared_ptr<const Buffer> _handle{};
    size_t _offset{};
    size_t _size{};
    void *_data{};

public:
    BufferDownloadCommand(std::shared_ptr<const Buffer> handle,
                          size_t offset_bytes, size_t size_bytes, void *data) noexcept
        : Command{},
          _handle{std::move(handle)}, _offset{offset_bytes}, _size{size_bytes}, _data{data} {}
    [[nodiscard]] auto handle() const noexcept { return _handle; }
    [[nodiscard]] auto offset() const noexcept { return _offset; }
    [[nodiscard]] auto size() const noexcept { return _size; }
    [[nodiscard]] auto data() const noexcept { return _data; }
    void accept(CommandVisitor &visitor) const noexcept override { visitor.visit(this); }
};

class BufferCopyCommand final : public Command {
private:
    std::shared_ptr<const Buffer> _src_handle{};
    std::shared_ptr<const Buffer> _dst_handle{};
    size_t _src_offset{};
    size_t _dst_offset{};
    size_t _size{};

public:
    BufferCopyCommand(std::shared_ptr<const Buffer> src, std::shared_ptr<const Buffer> dst,
                      size_t src_offset, size_t dst_offset, size_t size) noexcept
        : Command{},
          _src_handle{std::move(src)}, _dst_handle{std::move(dst)},
          _src_offset{src_offset}, _dst_offset{dst_offset}, _size{size} {}
    [[nodiscard]] auto src_handle() const noexcept { return _src_handle; }
    [[nodiscard]] auto dst_handle() const noexcept { return _dst_handle; }
    [[nodiscard]] auto src_offset() const noexcept { return _src_offset; }
    [[nodiscard]] auto dst_offset() const noexcept { return _dst_offset; }
    [[nodiscard]] auto size() const noexcept { return _size; }
    void accept(CommandVisitor &visitor) const noexcept override { visitor.visit(this); }
};

class CustomCommand : public Command {

public:
    explicit CustomCommand() noexcept = default;
    [[nodiscard]] virtual uint64_t uuid() const noexcept = 0;
    ~CustomCommand() noexcept override = default;
    void accept(CommandVisitor &visitor) const noexcept override { visitor.visit(this); }
};

}// namespace vox