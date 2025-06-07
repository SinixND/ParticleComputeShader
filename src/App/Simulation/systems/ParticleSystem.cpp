#include "ParticleSystem.h"

#include "Particle.h"
#include <raylib.h>
#include <raymath.h>

namespace ParticleSystem
{
    Particle const& attract(
        Particle& particle,
        Vector2 const& target,
        float multiplier
    )
    {
        Vector2 delta{ Vector2Subtract(
            target,
            particle.position
        ) };

        float distance = Vector2Length( delta );
        Vector2 direction = Vector2Normalize( delta );

        particle.velocity = Vector2Add(
            particle.velocity,
            Vector2Scale(
                direction,
                ( multiplier / distance )
            )
        );

        return particle;
    }

    Vector2 const& applyFriction(
        Vector2& velocity,
        float value
    )
    {
        velocity = Vector2Scale( velocity, value );

        return velocity;
    }

    Particle const& move(
        Particle& particle,
        float dt
    )
    {
        particle.position = Vector2Add(
            particle.position,
            Vector2Scale(
                particle.velocity,
                dt
            )
        );

        if ( particle.position.x < 0 || particle.position.x > GetScreenWidth() )
        {
            particle.velocity.x *= -1;
        }

        if ( particle.position.y < 0 || particle.position.y > GetScreenHeight() )
        {
            particle.velocity.y *= -1;
        }

        return particle;
    }

    void drawParticle( Vector2 const& position )
    {
        DrawPixelV(
            position,
            Color{
                255,
                255,
                255,
                255
            }
        );
    }
}

