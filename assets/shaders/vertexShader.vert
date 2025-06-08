#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 velocity;
layout(location = 2) in vec4 color;

uniform vec2 mousePosition;
uniform float multiplier;
uniform float friction;
uniform float dt;
uniform float screenWidth;
uniform float screenHeight;

out vec4 vertexColor;

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

    vec2 delta = target - position;

    //* Distance influences acceleration, fmax caps velocity delta
    float distance = max(length(delta), 100.0f);
    vec2 direction = normalize(delta);

    velocity = velocity + (direction * (multiplier / distance));

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
    vec2 newVelocity = attract(
            velocity,
            mousePosition,
            multiplier
        );

    newVelocity = applyFriction(
            newVelocity,
            friction
        );

    vec2 newPosition = move(
            position,
            velocity,
            screenWidth,
            screenHeight,
            dt
        );

    newPosition = normalize(newPosition);

    gl_Position = vec4(newPosition, 0.0, 1.0);

    vertexColor = color;
}
