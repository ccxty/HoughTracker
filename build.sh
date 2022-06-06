g++ -I include -fPIC -c src/args.cpp
g++ -shared -o libargs.so args.o

rm *.o
rm *.so
