#version 430

// This is the workgroup size. The largest size that is guaranteed by OpenGL
// to available is 1024, beyond this is uncertain.
// Might influence performance but only in advanced cases.
layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

// layout(location = 0) in vec2 position;
// layout(location = 1) in vec2 velocity;
// layout(location = 2) in vec4 color;
layout(std430, binding = 0) buffer PositionSSBO {
    vec4 pos[];
};

layout(std430, binding = 1) buffer VelocitySSBO {
    vec4 vel[];
};

layout(location = 0) uniform vec2 mousePosition;
layout(location = 1) uniform float multiplier;
layout(location = 2) uniform float friction;
layout(location = 3) uniform float dt;
layout(location = 4) uniform float screenWidth;
layout(location = 5) uniform float screenHeight;

//* Helper functions
vec2 attract(
    vec2 velocity,
    vec2 target,
    float multiplier
)
{
    if ((target.x == 0) && (target.y == 0))
    {
        return velocity;
    }

    vec2 distance2D = target - position;

    //* Distance influences acceleration, fmax caps velocity delta
    float distance1D = max(length(distance2D), 100.0f);
    vec2 direction = normalize(distance2D);

    velocity = velocity + (direction * (multiplier / distance1D));

    return velocity;
}

vec2 applyFriction(
    vec2 velocity,
    float value
)
{
    velocity = velocity * value;

    return velocity;
}

vec2 move(
    vec2 position,
    vec2 velocity,
    float screenWidth,
    float screenHeight,
    float dt
)
{
    position = position + (velocity * dt);

    if (position.x < 0)
    {
        position.x += screenWidth;
    }

    if (position.x > screenWidth)
    {
        position.x -= screenWidth;
    }

    if (position.y < 0)
    {
        position.y += screenHeight;
    }

    if (position.y > screenHeight)
    {
        position.y -= screenHeight;
    }

    return position;
}

void main()
{
    uint idx = gl_GlobalInvocationId.x; // Current invocation "position" in 3D Workgroup and local

    vec2 newPosition = pos[idx].xy;
    vec2 newVelocity = vel[idx].xy;

    newVelocity = attract(
            newVelocity,
            mousePosition,
            multiplier
        );

    newVelocity = applyFriction(
            newVelocity,
            friction
        );

    newPosition = move(
            newPosition,
            newVelocity,
            screenWidth,
            screenHeight,
            dt
        );

    newPosition = normalize(newPosition);

    //* Return
    pos[idx].xy = newPosition;
    vel[idx].xy = newVelocity;
}

