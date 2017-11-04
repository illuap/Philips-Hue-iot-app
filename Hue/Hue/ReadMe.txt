COMPILE WITH THE FOLLOWING:

g++ -std=c++11  log.cpp MyResource.cpp Hue.cpp -lwthttp -lwt -lboost_system -onchow24

EXECUTE WITH THE FOLLOWING:

./nchow24 --docroot /usr/local/share/Wt --http-address 0.0.0.0 --http-port 8080