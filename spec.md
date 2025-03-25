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
Use tags to categorize the issue, and use a deFEscriptive but short title. And then a longer, more in-depth description if needed.

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
This project is quite feasable. Executing lua code from C++ is easy. Integrating lua with the graphics and physics library will be the difficult part. We will need to learn and figure out a lot which may also contribute to difficulties. However, seeing as our MVP is quite simple we hope to finish this project fine.


### Who
- Olle
    - Integrating Lua
    - Physics engine (jolt)
- Oskar
    - Graphics library (bgfx)
    - CMake
    - Program architecture

## MVP

This is the features of a small "game" made
* Input - Apply force to the sphere
* Static plane
* RigidBody sphere

### Additional


### Lua Code

```lua

local Window = game:Get("Window")
Window.title = "MVP game"

local sphere = Primitive.new(Sphere) -- Default spawn position is (0,0,0)
sphere.BodyType = Enum.BodyType.RigidBody
sphere.Color = Rgb.new(255, 0, 0)

local planePosition = Vector3.new(0, -10, 0)
local plane = Primitive.new(Plane, planePosition) -- Primitive allows for optional default position as second argument
plane.Color = Rgb.new(0, 100, 100)

local Keyboard = game:Get("Keyboard")

Keyboard.KeyDown:Connect(function(key)
    if key == Enum.KeyCode.Space then
        print("Space Pressed!")
        sphere:ApplyImpulse(Vector3.new(0, 100, 0))
    end
end)

```