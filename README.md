# Pong Game

A classic Pong game implementation in C using the Windows API (WinAPI). Play against an AI opponent in this recreation of the iconic arcade game.

![image](https://github.com/user-attachments/assets/1ecf3754-f72d-4db0-840c-9c03ee349bd1)
![image](https://github.com/user-attachments/assets/f3e1fc5d-aa0a-4aee-b626-ba9f116a7c01)


## Features

- Clean, minimalist black and white design
- Smooth paddle and ball movement
- AI opponent with dynamic paddle movement
- Two control options:
  - Mouse control for right paddle
  - Keyboard control using arrow keys
- Score tracking
- Win condition (first to 10 points)
- Pause functionality
- Dynamic ball speed that increases during rallies
- Realistic paddle physics affecting ball direction

## Prerequisites

- Windows operating system
- C compiler (e.g., Visual Studio)
- Windows SDK for WinAPI development

## Installation

1. Clone the repository:
   ```
   git clone https://github.com/shellytrifonov/Pong-Game.git
   ```

2. Open the project in Visual Studio:
   - Open `pong.sln` solution file
   - Build the solution (F5 or Ctrl+F5)

## How to Play

### Controls

- **Main Menu**:
  - Press 'M' to select mouse control
  - Press 'K' to select keyboard control
  - ESC to quit game

- **During Game**:
  - **Mouse Control**: Move mouse up/down to control right paddle
  - **Keyboard Control**: Use UP/DOWN arrow keys to move right paddle
  - Press 'P' to pause/unpause
  - Press ESC to return to main menu

- **Win Screen**:
  - Press 'R' to play again
  - Press ESC to return to main menu

### Gameplay

- Control the right paddle to hit the ball back
- Left paddle is controlled by AI
- First player to reach 10 points wins
- Ball speed increases with each paddle hit
- Ball direction is influenced by where it hits the paddle

## Technical Details

- Window Size: 800x800 pixels
- Built using WinAPI for graphics and window management
- Double buffering implemented for smooth rendering
- Collision detection with paddle angle reflection
- AI opponent with predictive movement

## Contributing

Feel free to fork the repository and submit pull requests. You can also open issues for bugs or feature suggestions.

## License

This project is available under the MIT License. Feel free to use and modify the code for your own projects.
