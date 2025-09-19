# SD1-A02: Starship

## Overview

This project implements a starship game prototype featuring a player-controlled ship, asteroids, and projectiles. 
The game demonstrates fundamental game architecture patterns and utilizes OpenGL rendering.

## Architecture

### Core Systems
- App Class: Top-level application manager handling input and frame management
- Engine Class: Manages all engine subsystems such as Renderer
- Game Class: Gameplay manager and game logic
- Entity System: Abstract base class for PlayerShip, Asteroid, and Bullet


## Controls

| Key | Action |
|-----|--------|
| S | Turn left |
| F | Turn right |
| E | Thrust forward |
| Space | Fire bullet |
| N | Respawn ship |
| I | Spawn asteroid |
| P | Pause |
| T (hold) | Slow motion |
| O | Single frame step |
| F1 | Toggle debug |
| F8 | restart |
| Q | Quit |

### Debug Features
When F1 is pressed, debug rendering shows:
- Dark grey lines from ship to all entities
- Magenta circles for cosmetic radius
- Cyan circles for physics radius
- Red lines for forward vectors
- Green lines for left vectors
- Yellow lines for velocity vectors

### Running
- To run the program you need to grab the .exe file that is in the run folder