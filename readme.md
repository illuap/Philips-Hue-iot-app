Team 5 project!
g++ -std=c++14 -o a.out QRAuth.c QRAuthWidget.c ./model/QRAuthService.c ./model/QRTokenDataBase.c ./model/Session.c ./model/User.c -I/usr/local/include -L/usr/local/lib -lwthttp -lwt -lboost_system -lwtdbo -lwtdbosqlite3

./test.out --docroot ./ --http-address 127.0.0.1 --http-port 8080

java -jar HueEmulator-v0.7.jar
