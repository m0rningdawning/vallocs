//
// Created by Paul on 21/11/2025.
// This is AI generated and needs to be fixed!
//

#ifndef VALLOCS_BUMP_SERVICE_H
#define VALLOCS_BUMP_SERVICE_H

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "../allocs/bump/bump_allocator.h"
#include "allocator.grpc.pb.h"  // from generated/

namespace vallocs::rpc {

class BumpAllocatorServiceImpl final : public BumpAllocatorService::Service {
public:
    BumpAllocatorServiceImpl();

    ::grpc::Status CreateArena(::grpc::ServerContext* context,
                               const CreateArenaRequest* request,
                               CreateArenaResponse* response) override;

    ::grpc::Status Allocate(::grpc::ServerContext* context,
                            const AllocateRequest* request,
                            AllocateResponse* response) override;

    ::grpc::Status Reset(::grpc::ServerContext* context,
                         const ResetRequest* request,
                         ResetResponse* response) override;

private:
    using Arena = vallocs::bump::bump_allocator<char>;

    std::mutex mutex_;
    std::uint64_t next_id_{1};
    std::unordered_map<std::uint64_t, std::unique_ptr<Arena>> arenas_;
};

} // namespace vallocs::rpc

#endif //VALLOCS_BUMP_SERVICE_H