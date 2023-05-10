echo Containers available before cleanup
docker ps -a

docker stop conan_env
docker rm conan_env

echo Current containers available
docker ps -a