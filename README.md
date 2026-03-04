# 2D Ship Builder Breakout (C++ / OpenGL)

A 2D game built in **C++** using an OpenGL-based framework.  
The player first **builds a spaceship** in a grid (drag & drop components) and then plays a **Breakout-style** level where a ball destroys blocks.

## Gameplay

### 1) Build Mode (Grid)
- Drag & drop ship parts into a **12x8 grid**
- Available parts: **engine**, **bumper**, **square**, **gun**
- Max **10 blocks** can be placed
- Ship color can be changed using color buttons
- The ship must be **valid**:
  - blocks must be connected (**BFS connectivity check**)
  - placement rules for engine/gun/bumper are enforced
- Press the green **START** button to begin

### 2) Game Mode
- Press **SPACE** to launch the ball
- Move ship with **LEFT / RIGHT**
- Break blocks to increase score
- Lose a life if the ball falls below the ship
- Visual effects:
  - **screen shake** when a block breaks
  - **particle background**
- Pause with **P**
- End screen shows win/lose + score  
  - **SPACE**: rebuild ship  
  - **ENTER**: exit

## Controls

- **Mouse Left Click + Drag**: pick and place ship parts
- **Mouse Right Click** (in grid): remove placed parts
- **LEFT / RIGHT**: move ship (during game)
- **SPACE**: launch ball / restart from final screen
- **P**: pause/unpause
- **BACKSPACE**: go back to previous screen
- **ENTER**: exit (final screen)

## Implementation Highlights

- **Orthographic camera** + custom 2D transformations (`transform2D`)
- Grid building + validation rules
- **BFS** used to ensure all placed blocks are connected
- Ball collision with blocks and ship (angle based on hit position)
- Animated block destruction + camera shake effect
- Background particle system
