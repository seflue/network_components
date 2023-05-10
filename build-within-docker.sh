proj_path=$(pwd)
proj_name=$(basename "${proj_path}")
docker run --rm \
  -v "${proj_path}:/tmp/${proj_name}" \
  -w "/tmp/${proj_name}" \
  "${proj_name}/conan-env:1.0" \
  "/bin/bash" "./build.sh" "$@" --env "Docker"
