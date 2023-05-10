# TODO
- [x] configure CLion WSL build with CMake
- [x] include grpc and POCO
- [x] get simplest GRPC communication up and running
- [x] initialize git repo
- [x] initially design interaction between controlBand and user plane
 
- [ ] implement multiple base stations
  - [x] make station a struct with address string and quality as int
  - [x] allow multiple stations within UserEquipment
    - [x] adjust class structure and construction
    - [x] adjust scan method
    - [ ] repair broken channel construction
  - [ ] start multiple base stations
- [ ] implement user plane
  - [x] make Poco headers successfully available
  - [ ] add DataHandler, 
    - [x] checks, if data is available (return random value)
  - [ ] implement simple UDP data transfer
- [ ] Cleanup build
  - [ ] Cleanup docker build
    - [x] remove bincrafters remote
    - [ ] pass existing conanfile.txt to conan_cmake_configure
  - [ ] cleanup build scripts
    - [ ] Write documentation for build.sh
    - [ ] Exchange Wsl env parameter option with something more windows agnostic
  - [ ] consolidate CLion build
    - [ ] repair conan cmake lookup when called from CLion
    - [ ] use preset files to persist CLion CMake configuration
- [ ] remove busy-waiting for timer checking
  - [ ] evaluate using a state machine
  - [ ] move timer into ControlClient and start scanning as separate thread
- [ ] make channel part of UE instance
- [ ] parallelize connection (parallel std::for_each)
- [ ] implement "Connect"
- [ ] fix conditional include in CMakeLists.txt
- [ ] add clang-format configuration