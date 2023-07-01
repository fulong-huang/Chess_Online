# Chess_Online [In Process]
<br/>

## Description:
Game of Chess, but can be played with more than two players.
* Unlimited amount of players.
* Anyone can move any piece, as long as the movement is valid.
* Each move will cost a movement point, whether the movement is valid or not.
* Server can set cooldown time and maximum movement points holding.
* Only valid moves will be shown on the client side.

***
 TODO:
* ~~(*) Server commands while game running, such as set cooldown.~~
* ~~(*) User interface: Movement points~~.
* ~~(*) User interface: Assign and display team~~.
* (*) UI for changing pawn pomotion type (Currently default to Queen).
* (*) TBD.

## How to RUN:  (Game not complete, likely will not function as expected)
&emsp;&emsp; Running the following command from the root directory to target CMAKE build directory:
```
    cmake -S . -B <Target Build Directory>
```
&emsp;&emsp; Go into the build directory, build the game with:
```
    make
```
&emsp;&emsp; For server, start the server with:
```
    ./Server <Port Number>
```
&emsp;&emsp; For clients, start the game with:
```
    ./Client <Server IP> <Server Port> <Player Name>
```
&emsp;&emsp; Not that Player Name will be striped down to 10 characters.

***
## Not Working? Try these:
* Server IP should be the public IP address from the server side (unless running locally).<br/>
* Make sure cmake was installed, and included in environment variables. <br/>
* Make sure the port was open and available, might need to forward the port to the server machine.<br/>


