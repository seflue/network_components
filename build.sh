type="Release"
env="Wsl"
local_conan=false
local_conan_dir="$(pwd)/local_conan"
run_conan=false

b() {
  echo "$(tput bold)$1$(tput sgr0)"
}

hl() {
  echo "$(tput setaf 2)$1$(tput sgr0)"
}

usage() {
  echo "This script builds the whole project using cmake. It is made "
  echo ""
  echo "Usage: $0 [-t|--type=Release|Debug] [-e|--env=Wsl|Docker] [-l|--local-conan] [-c|--run-conan]"
  echo ""
  echo ""
  echo "Options:"
  echo "  -t, --type          Defines the built type for cmake and can be set to $(hl "Release") or $(hl "Debug")."
  echo "                      The value is forwarded to cmake as $(hl -DCMAKE_BUILD_TYPE) and also encoded in the build"
  echo "                      directory name."
  echo "                      Default: $(hl "Release")"
  echo "                      "
  echo "  -e, --env           Specifies, in which environment the script is running."
  echo "                        $(hl "Wsl")     Runs in the shell it was started from."
  echo "                        $(hl "Docker")  build.sh is expected to be running within a docker container."
  echo "                      Default: $(hl "Release")"
  echo "                      "
  echo "  -l, --local-conan   Set $(hl CONAN_USER_HOME)=$(hl $(pwd)/local_conan)"
  echo "                      Within a docker environment CONAN_USER_HOME needs to be mounted into the container to"
  echo "                      avoid huge rebuilds everytime. This parameter allows to use a separate cache, which does"
  echo "                      not interfere with conan's default cache."
  echo "                      Default: not set"
  echo "                      "
  echo "  -c, --run-conan     If set, conan is run before starting cmake."
  echo "                      Default: not set"
}

while [[ "$#" -gt 0 ]]; do
  case "$1" in
    -t|--type) type="$2"; shift ;;
    -e|--env)  env="$2"; shift ;;
    -l|--local-conan) local_conan=true ;;
    -c|--run-conan) run_conan=true ;;
    -h|--help) usage
       exit 0 ;;
    *) echo "Invalid argument: $1" >&2
       usage
       exit 1 ;;
  esac
  shift
done

# Validate arguments
case "$type" in
  "Release") ;;
  "Debug") ;;
  *)
    echo "Invalid argument for --type, it must be either Debug or Release"
    exit 1
    ;;
esac

case "$env" in
  "Docker") ;;
  "Wsl") ;;
  *)
    echo "Invalid argument for --env, it must be either Wsl or Docker"
    exit 1
    ;;
esac

tmp_conan_user_home="${CONAN_USER_HOME}"
build_dir="cmake-build-${type,}-${env,}"

if [[ $local_conan == true ]]; then
  echo "CONAN_USER_HOME is '${tmp_conan_user_home}'"
  echo "Set CONAN_USER_HOME to ${local_conan_dir}."
  export CONAN_USER_HOME="${local_conan_dir}"
fi

export CONAN_REVISIONS_ENABLED=1

echo "Create and go to folder ${build_dir}"
mkdir -p "${build_dir}" && pushd "${build_dir}"

[[ $run_conan == true ]] && conan install .. --build=missing

cmake .. -G Ninja "-DCMAKE_BUILD_TYPE=${type}"
cmake --build . -- -j 10
popd

[[ $local_conan == true ]] && export CONAN_USER_HOME="${tmp_conan_user_home}"