docker run \
  --rm `# cleanup after run` \
  -d \
  --cap-add sys_ptrace \
  -p 127.0.0.1:2222:22 \
  --name conan_env \
  -t `# start tty` \
  network_components/conan-env:1.0
