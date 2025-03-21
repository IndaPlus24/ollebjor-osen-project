# Specification

### Repo
[Link to repo](https://github.com/IndaPlus24/ollebjor-osen-project)

### Naming conventions
#### Commit
> ACTION: <`short-description`>

Where ACTION is:
- FEAT - New feature
- FIX - Fixed something
- DOCS - Updated documentation
- REF - Refactored code
- ADD - Added something new that is not a feature
- MISC - Miscellanous 

#### Issues
Use tags to categorize the issue, and use a descriptive but short title. And then a longer, more in-depth description if needed.

Otherwise, fall back to commit convention.

#### Pull requests
Use commit convention as title. And then tag the relevant issues this merge solves.

### What
#### Features

- Physics simulation with [jolt](https://github.com/jrouwe/JoltPhysics)
    - Primitives
    - Collision
    - Trigger volumes
    - (Optional) Character movement
- Rendering with [bgfx](https://github.com/jrouwe/JoltPhysics)
    - Color
    - Models
    - Primitives
- Super awesome [lua](https://www.lua.org/) api to add functionality and customization
    - Spawn primitives
    - Customize settings
    - Define behaviour

#### Feasability


### Who

MVP Code

```lua

local Window = game:Get("Window")
Window.title = "MVP game"

local spherePosition = Vector3.zero

```

