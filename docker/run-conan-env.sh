mount_conan=false
interactive=false

usage() {
  echo "Usage: $0 [-c|--mount-conan] [-i] [<command>]"
  echo
  echo "Runs a command within the docker build container."
  echo
  echo "Options:"
  echo "  -c, --mount-conan  Mounts the local conan folder into the container."
  echo "  -i, --interactive  Starts the container in an interactive mode."
}

while [[ "$#" -gt 0 ]]; do
  case "$1" in
    -c|--mount-conan) mount_conan=true ;;
    -i|--interactive) interactive=true ;;
    *) echo "Invalid argument $1" >&2
      usage
      exit 1 ;;
  esac
  shift
done

[[ interactive == true ]] && terminal="-it" || terminal="-t"
[[ interactive == true ]] && command="/bin/bash" || command=""
[[ mount_conan == true ]] && mountpoint="-v $(pwd)/../local_conan:/tmp/conan_files" || mountpoint=""

docker run \
  --rm `# cleanup after run` \
  -d \
  --cap-add sys_ptrace \
  -p 127.0.0.1:2222:22 \
  --name conan_env \
  "$mountpoint" \
  "$termial" `# start tty` \
  network_components/conan-env:1.0
