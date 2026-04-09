# Intro to C++ and Unreal - Snake Game
Project for converting a terminal snake game written in C++ to an Unreal Engine game.
Unreal version 5.7.4

## GDD Base
 - Grid: Generated play area. Will have 3 levels.
   - 1st level is tunnels.
   - 2nd level is general play area.
   - 3rd level is bridges.
 - Stateful Game flow.
    - Start on Main Menu
    - Game state
    - Outro state
 - 2 player
   - Either split screen or network play
   - One of two modes:
     - Cooperative mode: Common goal for clearing stage
     - Battle mode: player try to grow longer and eliminate opponent
 - Movement, spacial and progression requirements:
   - Must be in 3D
   - Dynamic camera; camera needs to follow snake somehow
   - Snake movement must be smooth or interpolated
   - 3 levels of difficulty
   - Stage progression must increase in difficulty
     - Difficulty incresement options (minimum 2):
       - Snake speed
       - wall density or layout
       - food rules
       - hazards
       - arena size
     - Data driven stage progression reccommended.

## Completed:
    Sprint 1 objectives:
    - Snake pawn
    - Basic snake movement on grid
    - Basic grid generation

## In Progress:
    Sprint 2 objectives:
    - Core snake loop
        - Snake can move, eat, grow and die
            - Create spawner for food
            - Create Manager for game score and management
    - Collision model
        - Hybrid collision model
            - Walls and other grid items (bridges and ramps); managed by grid
            - Unreal collisions for self collision and food (self collisions might be handled in grid as well)
    - State Flow and UI
        - Basic state machine for MainMenu, Game and Outro

## Overall Steps:
1. Single-player snake: movement, growth, food, and death
2. State machine: MainMenu, Game, Outro
3. Stage progression: at least 3 stages, ideally data-driven
4. Split-screen for 2 players
5. Add one game mode: Cooperative or Battle
6. Add polish: UI, particles, audio, and lighting tied to events
7. Add AI and/or the second mode, then the VG depth extension if you are aiming for VG
## Assignment Details
- Unreal Engine and C++ requirements
  - Use Unreal Engine's collision system for gameplay interactions such as snake-wall, snake-snake, and
           food interactions.
    - All core gameplay logic must be implemented in C++ and exposed to Blueprint where appropriate, for
        example with UPROPERTY, UFUNCTION, BlueprintCallable, BlueprintReadOnly, or
        BlueprintImplementableEvent.
    - Level walls must be generated, either procedurally or from data. They must not be placed manually
    one by one throughout the level.
    - Use an event-driven approach where appropriate. Do not solve everything in Tick. Good uses of
    delegates, events, and callbacks include score updates, death handling, stage completion, and
    game-mode transitions.
    - Use the following elements in a way that is connected to gameplay or game states, not merely present
    in the project:
    - Widgets: MainMenu, in-game HUD, and Outro
    - Particle effects, for example on eating, death, or stage completion
    - Audio, for example on eating, death, or UI actions
    - Multiple light sources, with at least one used purposefully for readability or atmosphere
### What is expected for G
-  To reach G, the project should demonstrate a complete and playable version of the assignment.
   - The game runs, compiles cleanly, and is playable.
   - The three-state flow works and the restart loop is functional.
   - At least 2 players can play simultaneously.
   - The game includes smooth snake movement in 3D, meaningful stage progression, and working
   collisions through Unreal's collision system.
   - Core gameplay logic is implemented in C++.
   - Walls are generated.
   - Widgets are functional.
   - Particles and audio are triggered by gameplay events.
   - Light sources are used mostly purposefully.
   - The code is readable, follows Unreal conventions, and shows sensible class roles and naming.
### Requirements for VG
   For VG, the project should go beyond a minimal pass and present a fuller, more complete game with
   stronger technical design, broader feature scope, and better polish and stability.
   Choose advanced extensions
   - To aim for VG, you should normally implement all of the following:
   - Both game modes: Cooperative and Battle
   - AI player support, where each player slot can be human or AI and the AI uses an AController with
   credible behavior
   - A third depth dimension in the level design, implemented in a way that remains playable and
   understandable
   You may also propose your own VG extension, provided it is comparable in scope and difficulty.
   3D depth requirement for VG
   - If you implement the VG depth extension, the game must remain fully playable in 3D.
   - Core features that worked in the 2D-layer version must still work, including states, scoring, collisions,
   stage progression, and UI flow.
   - Two players must still be able to play at the same time.
   - If AI is implemented, it must still be able to navigate in the 3D version.
   - The added depth must be clearly visualized for the player, for example through camera design,
   transparent layers, fading or slicing, a minimap or layer indicator, or another well-motivated solution.
   Design and code expectations for VG
   - Stronger separation of concerns between GameMode, GameState, Controllers, Pawn, UI, and
   supporting systems.
   - Progression and balancing should preferably be data-driven and easy to tweak.
   - An event-driven approach should be used consistently, with minimal overuse of Tick.
   - The project should feel complete: readable UI, clear feedback, stable restart loop, predictable
   gameplay, and few or no major bugs.