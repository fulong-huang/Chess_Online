# Chess_Online [In Process]
<br/>

## Description:
&emsp;&emsp; Game of Chess, but it's online and weird.
***
 Features marked with (*) mean it is not implemented yet.
* Unlimited amount of players.
* Anyone can move any piece, as long as the movement is valid.
* (*) Each move will cost a movement point, whether it is valid or not.
* (*) Server will set cooldown time and maximum holding movement points.
* Only valid moves will be shown on the client side.
<br/><br/>

## How to RUN:  (Game not complete, likely will not function as expected)
&emsp;&emsp; running the following command from the root directory to target built location:
<br/>
```
    cmake -S . -B <Target Build Directory>
```
<br/>&emsp;&emsp; Go into the build directory, build the game with:
```
    make
```
&emsp;&emsp; For server, start the server with:
```
    ./Server <Port Number>
```
&emsp;&emsp; For clients, start the game with:
```
    ./Client <Server IP> <Server Port>
```

## Not Working? Try these:
***
* Server IP should be the public IP address from the server side (unless running locally).<br/>
* Make sure cmake was installed, and included in environment variables. <br/>
* Make sure the port was open and available, might need to forward the port to the server machine.<br/>


