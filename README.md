# vallocs
Various mamory allocators implemented in C++

### TODO
- [ ] Bump Allocator
  - [x] Create a basic, win version of the allocator
  - [x] Abstract it + make linux hooks for cross-platform
  - [x] Put templates on it
  - [x] Make it iterable
  - [x] Add protection
  - [x] Possible growth strats
  - [x] Test it with gtest
  - [ ] Try to incorporate grpc/protobuf in it
  - [ ] Clean-up, review, list of improvements, move on.
- [ ] Stack Allocator
  - [x] Basic alloc
  - [ ] Proper alloc
  - [ ] Test with gTest
