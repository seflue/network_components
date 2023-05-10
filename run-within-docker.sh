proj_path=$(pwd)
proj_name=$(basename "${proj_path}")

local_canon_user_home="$(pwd)/local_conan"
docker_canon_user_home="/tmp/conan_packages"
docker run --rm \
  -v "${proj_path}:/tmp/${proj_name}" \
  -v "${local_canon_user_home}:${docker_canon_user_home}" \
  -w "/tmp/${proj_name}" \
  "${proj_name}/conan-env:1.0" "$@"
