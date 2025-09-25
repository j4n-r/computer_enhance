
# e.g. "./build.sh 1_6"

g++ -I.. ${1}.cpp ../sim86_lib.so -Wl,-rpath,../ -o ${1}
