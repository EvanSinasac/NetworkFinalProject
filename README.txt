INFO6019 Network Programming Final Project
Evan Sinasac - 1081418

Build and run in Microsoft Visual Studios Community 2019 in Debug and Release in x64.

INSTRUCTIONS:
I have been building the project in Visual Studio, it's set up to build the server then the client if you build the client, then running the executables in the output directory.  All four players must connect to the server and have pressed "Enter" for the game to begin.
Pressing 1-4 in the GUI window of any client will change that client's update frequency to 1Hz, 10Hz, 60Hz and 100Hz respectively.  Pressing the same buttons in the server's console window will do the same for the server.  In the client's GUI windows, pressing 5 will toggle the client side prediction, and pressing 6 will turn on dead reckoning.  
The client side prediction basically allows the client to move the mesh freely and let's the server handle updating the other clients.  When it's off, pressing a directional key will tell the server where to move that client and then update all the clients.  I honestly can't tell if the dead reckoning is actually working, but the logic is there for it to.


Players 1 and 2 control the vertical paddles on the left and right of the screen, so in their clients, pressing up/down arrow keys will move them.
Players 3 and 4 control the horizontal paddles on the top and bottom of the screen, so in their clients, pressing left/right arrow keys will move them.

github link: https://github.com/EvanSinasac/NetworkingFinalProjectEvanSinasac
github backup: https://github.com/EvanSinasac/NetworkFinalProject

VIDEO
https://youtu.be/fKN3A9C61Eg

