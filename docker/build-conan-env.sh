#!/usr/bin/env bash
cache=""
image_name="network_components/conan-env:1.0"
skip_conan=false

usage() {
  echo "Usage: $0 [-r|--no-cache] [-s|--skip-conan"
  echo
  echo "Options:"
  echo "  -r, --no-cache   Rebuilds the whole container, resetting cached layers."
  echo "                   (see docker build --no-cache)"
  echo "  -s, --skip-conan Omits extracting conan information into local conan folder."
}

while [[ "$#" -gt 0 ]]; do
  case "$1" in
    -r|--no-cache) cache="--no-cache" ;;
    -s|--skip-conan) skip_conan=true ;;
    -h|--help) usage
      exit 0 ;;
    *) echo "Invalid argument: $1" >&2
      usage
      exit 1 ;;
  esac
  shift
done

echo "Build dev container ..."
docker build \
   ${cache} \
   `#--progress=plain` \
   -t "${image_name}" \
   -f Dockerfile.conan-env .

if [[ "$skip_conan" == true ]]; then
  echo "Skipping local conan preparation."
  exit 0;
fi

echo "Prepare local conan folder ..."
docker run --rm --entrypoint="printenv" "${image_name}" "CONAN_USER_HOME"
docker_conan_user_home=$(docker run --rm --entrypoint="printenv" "${image_name}" "CONAN_USER_HOME" )

pushd .. 
local_conan="$(pwd)/local_conan"
popd || return

conan_profiles=".conan/profiles"
local_conan_profiles="$local_conan/$conan_profiles"
mkdir -p "$local_conan_profiles"

docker create --name dummy "${image_name}"
mkdir -p ../local_conan/.conan/profiles
docker cp "dummy:${docker_conan_user_home}/${conan_profiles}/default" "$local_conan_profiles"
docker rm -f dummy
