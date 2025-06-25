#version 430 core

// layout(location = 0) in vec2 position;
// layout(location = 1) in vec4 color;

layout(std430, binding = 0) buffer positionSSBO {
    vec4 pos[];
};

layout(std430, binding = 1) buffer velocitySSBO {
    vec4 vel[];
};

layout(std430, binding = 2) buffer colorSSBO {
    vec4 col[];
};

layout(location = 3) uniform vec2 mousePosition;
layout(location = 4) uniform float multiplier;
layout(location = 5) uniform float friction;
layout(location = 6) uniform float dt;
layout(location = 7) uniform float screenWidth;
layout(location = 8) uniform float screenHeight;

out vec4 vertexColor;

//* Helper functions
vec2 attract(
    vec2 position,
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
    gl_PointSize = 5.0f;
    int idx = gl_VertexID; // Current vertex ID

    vec2 newPosition = pos[idx].xy;
    vec2 newVelocity = vel[idx].xy;
    vec4 newColor = col[idx];

    newVelocity = attract(
            newPosition,
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
    col[idx] = newColor;

    gl_Position = vec4(newPosition, 0.0, 1.0);

    vertexColor = newColor;
}
