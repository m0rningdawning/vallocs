//
// Created by Paul on 21/11/2025.
// This is AI generated and needs to be fixed!
//

#include "bump_service.h"
#include <grpcpp/grpcpp.h>

namespace vallocs::rpc {
    BumpAllocatorServiceImpl::BumpAllocatorServiceImpl() = default;

    ::grpc::Status BumpAllocatorServiceImpl::CreateArena(
        ::grpc::ServerContext*,
        const CreateArenaRequest* request,
        CreateArenaResponse* response) {
        const std::uint64_t capacity = request->capacity();
        if (capacity == 0) {
            return ::grpc::Status(::grpc::StatusCode::INVALID_ARGUMENT, "capacity must be > 0");
        }

        std::lock_guard<std::mutex> lock(mutex_);

        const std::uint64_t id = next_id_++;
        try {
            auto arena = std::make_unique<Arena>(static_cast<std::size_t>(capacity));
            arenas_.emplace(id, std::move(arena));
        }
        catch (const std::bad_alloc&) {
            return ::grpc::Status(::grpc::StatusCode::RESOURCE_EXHAUSTED,
                                  "failed to allocate arena backing memory");
        }

        response->set_arena_id(id);
        return ::grpc::Status::OK;
    }

    ::grpc::Status BumpAllocatorServiceImpl::Allocate(
        ::grpc::ServerContext*,
        const AllocateRequest* request,
        AllocateResponse* response) {
        const std::uint64_t arena_id = request->arena_id();
        const std::uint64_t size = request->size();

        if (size == 0) {
            return ::grpc::Status(::grpc::StatusCode::INVALID_ARGUMENT, "size must be > 0");
        }

        std::lock_guard<std::mutex> lock(mutex_);

        auto it = arenas_.find(arena_id);
        if (it == arenas_.end()) {
            return ::grpc::Status(::grpc::StatusCode::NOT_FOUND, "arena not found");
        }

        Arena& arena = *it->second;
        const std::size_t before = arena.get_marker();

        char* ptr = arena.allocate(static_cast<std::size_t>(size));
        if (!ptr) {
            return ::grpc::Status(::grpc::StatusCode::RESOURCE_EXHAUSTED, "arena out of memory");
        }

        const std::size_t after = arena.get_marker();
        const std::size_t offset = before; // allocation starts at previous marker

        response->set_arena_id(arena_id);
        response->set_offset(static_cast<std::uint64_t>(offset));
        (void)after; // kept for clarity; could be used for debug/logging

        return ::grpc::Status::OK;
    }

    ::grpc::Status BumpAllocatorServiceImpl::Reset(
        ::grpc::ServerContext*,
        const ResetRequest* request,
        ResetResponse* response) {
        const std::uint64_t arena_id = request->arena_id();

        std::lock_guard<std::mutex> lock(mutex_);

        auto it = arenas_.find(arena_id);
        if (it == arenas_.end()) {
            return ::grpc::Status(::grpc::StatusCode::NOT_FOUND, "arena not found");
        }

        it->second->reset();
        response->set_ok(true);
        return ::grpc::Status::OK;
    }
} // namespace vallocs::rpc
