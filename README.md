# Chess_Online (MacOS)
<br/>

## Description:
Game of Chess, but can be played with more than two players.
* Unlimited amount of (recommanded <= 10 to avoid visual bugs) players.
* Each move will cost a movement point, whether the movement is valid or not.
* Anyone can move any piece, as long as the movement is valid.
* Server can set cooldown time and maximum movement points of players.
* Only valid moves will be shown on the client side UI.
* Client can command lines to move, or request board if out of sync.
* Client need to select pawn promotion type before movement was placed.

## How to RUN: 
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
&emsp;&emsp; Then use ```cooldown <time>``` to set cooldown (ms), ```move <count>``` to set moves player can hold, or ```start``` to start game.

&emsp;&emsp; For clients, start the game with:
```
    ./Client <Server IP> <Server Port> <Player Name>
```
&emsp;&emsp; Note that Player Name will be striped down to 10 characters.

## Argument unused during compilation?
&emsp;&emsp; Try go into ```<build directory>/_deps/sfml_src/cmake/Macros.cmake```, in line 27 and 28, remove ```"-stdlib=libc++"```. Now save the file and try again.
<br><br>
&emsp;&emsp; Line 27 and 28 should be:
```
target_compile_options(${target} PRIVATE)
target_link_libraries(${target} PRIVATE)
```

***
## Not Working? Try these:
* Server IP should be the public IP address from the server side (unless running locally).<br/>
* Make sure cmake was installed, and included in environment variables. <br/>
* Make sure the port was open and available, might need to forward the port to the server machine.<br/>


