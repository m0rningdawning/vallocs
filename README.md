# vallocs
Various mamory allocators implemented in C++

### TODO
- [ ] Arena Allocator
  - [x] Create a basic, win version of the allocator
  - [ ] Abstract it + make linux hooks for cross-platform
  - [x] Put templates on it
  - [ ] Make it iterable
  - [ ] Add protection
  - [ ] Container support
  - [ ] Possible growth strats
  - [ ] Test it with gtest
  - [ ] Try to incorporate grpc/protobuf in it
  - [ ] Clean-up, review, list of improvements, move on.
- [ ] Stack Allocator
- [ ] Free-List Allocator
- [ ] Pool Allocator
- [ ] Buddy Allocator