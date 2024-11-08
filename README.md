# IsleaAI

> [!CAUTION]  
> Using TSharedPtr in this case was a major mistake. Unfortunately, I didn’t have enough knowledge when I made this decision, but now I understand that it was a significant error.
> So, I'm leaving the code as it is, and now you’re aware of the issue.

## Overview

This project is a **Trading Fleet Simulator** where players manage a fleet of ships navigating between islands, trading goods at various ports. The game features an overhead view similar to strategy games and offers both manual and automated trading systems. 

### Key Features:
- **Map**: The game map consists of a sea with islands. Players view the map from above.
- **Trading Ports**: Some islands feature trading ports where goods can be bought and sold. Prices fluctuate randomly by +/- 30% each week.
- **Ship Navigation**: Ships can travel between ports, taking between 1 and 14 days, where 1 day equals 5 seconds in real time.
  - Ships must avoid islands that obstruct their direct path.
- **Automated Trade**: While a ship is docked, players can manually choose goods and ports or opt for an automated mode.
  - In automated mode, the ship will choose the most profitable route and goods, considering travel distance and other factors.
- **Cargo Limit**: Each ship has limited cargo space.
- **Port Resources**: The amount of resources available for purchase or sale in ports is limited and changes daily.
- **Starting Conditions**: The player starts with one ship and 10,000 gold to begin their trading empire.
- **Automated Mode**: Players can enable a full automation mode where ships autonomously earn money without player intervention.
- **Camera Movement**: The camera can be moved with the WASD keys.

## Additional Features

### Additional Task 1 (✅ Completed):
- **Hire New Ships**: Players can hire new ships at various ports to expand their trading fleet.

### Additional Task 2 (* ❌ Not Completed):
- **Pirate and Storm Zones**: Add pirate and storm zones to the map and implement their effects on the player's trading empire.

### Additional Task 3 (✅ Completed):
- **Loading/Unloading Time**: Implement a system where ships take time to load/unload goods, represented by a progress bar.

### Additional Task 4 (*** ❌ Not Completed):
- **Save/Load Feature**: Allow players to save and load their progress, preserving their trading empire.
