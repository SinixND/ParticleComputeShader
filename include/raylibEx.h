#ifndef IG20240203171045
#define IG20240203171045

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <utility>

//* Typedefs

class RectangleEx
{
    float left_{};
    float top_{};
    float right_{};
    float bottom_{};
    float width_{};
    float height_{};

public:
    RectangleEx() = default;

    RectangleEx(
        float left,
        float top,
        float width = 0,
        float height = 0
    )
        : left_( left )
        , top_( top )
        , right_( left + width )
        , bottom_( top + height )
        , width_( width )
        , height_( height )
    {
        validate();
    }

    explicit RectangleEx(
        Vector2 const& topLeft,
        Vector2 const& bottomRight = { 0, 0 }
    )
        : left_( topLeft.x )
        , top_( topLeft.y )
        , right_( bottomRight.x )
        , bottom_( bottomRight.y )
        , width_( bottomRight.x - topLeft.x )
        , height_( bottomRight.y - topLeft.y )
    {
        validate();
    }

    RectangleEx(
        Vector2 const& center,
        float width,
        float height
    )
        : left_( center.x - ( 0.5f * width ) )
        , top_( center.y - ( 0.5f * height ) )
        , right_( center.x + ( 0.5f * width ) )
        , bottom_( center.y + ( 0.5f * height ) )
        , width_( width )
        , height_( height )
    {
        validate();
    }

    explicit RectangleEx( Rectangle const& rectangle )
        : left_( rectangle.x )
        , top_( rectangle.y )
        , right_( rectangle.x + rectangle.width )
        , bottom_( rectangle.y + rectangle.height )
        , width_( rectangle.width )
        , height_( rectangle.height )
    {
        validate();
    }

    float left() const
    {
        return left_;
    }

    float right() const
    {
        return right_;
    }

    float top() const
    {
        return top_;
    }

    float bottom() const
    {
        return bottom_;
    }

    float width() const
    {
        return width_;
    }

    float height() const
    {
        return height_;
    }

    Vector2 topLeft() const
    {
        return Vector2{
            left_,
            top_
        };
    }

    Vector2 bottomRight() const
    {
        return Vector2{
            right_,
            bottom_
        };
    }

    Vector2 center() const
    {
        return Vector2{
            ( left_ + ( 0.5f * width_ ) ),
            ( top_ + ( 0.5f * height_ ) )
        };
    }

    Rectangle rectangle() const
    {
        return Rectangle{
            left_,
            top_,
            width_,
            height_
        };
    }

private:
    void updateWidth()
    {
        width_ = right_ - left_;
    }

    void updateHeight()
    {
        height_ = bottom_ - top_;
    }

    void validate()
    {
        if ( right_ < left_ )
        {
            float temp{ right_ };
            right_ = left_;
            left_ = temp;
        }

        if ( bottom_ < top_ )
        {
            float temp{ bottom_ };
            bottom_ = top_;
            top_ = temp;
        }
    }
};

struct Vector2I
{
    int x;
    int y;
};

class RectangleExI
{
    int left_{};
    int top_{};
    int right_{};
    int bottom_{};
    int width_{};
    int height_{};

public:
    RectangleExI() = default;

    RectangleExI(
        int left,
        int top,
        int width = 1,
        int height = 1
    )
        : left_( left )
        , top_( top )
        , right_( left + width - 1 )
        , bottom_( top + height - 1 )
        , width_( width )
        , height_( height )
    {
        validate();
    }

    explicit RectangleExI(
        Vector2I const& topLeft,
        Vector2I const& bottomRight = { 0, 0 }
    )
        : left_( topLeft.x )
        , top_( topLeft.y )
        , right_( bottomRight.x )
        , bottom_( bottomRight.y )
        , width_( bottomRight.x - topLeft.x + 1 )
        , height_( bottomRight.y - topLeft.y + 1 )
    {
        validate();
    }

    //* Only odd values for width/height
    RectangleExI(
        Vector2I const& center,
        int width,
        int height
    )
        : left_( center.x - ( width / 2 ) )
        , top_( center.y - ( height / 2 ) )
        , right_( center.x + ( width / 2 ) )
        , bottom_( center.y + ( height / 2 ) )
        , width_( width )
        , height_( height )
    {
        validate();
#if defined( DEBUG )
        assert( ( width % 2 ) && ( height % 2 ) && "[ERROR] Width or height invalid; Must be odd value!" );
#endif
    }

    int left() const
    {
        return left_;
    }

    int right() const
    {
        return right_;
    }

    int top() const
    {
        return top_;
    }

    int bottom() const
    {
        return bottom_;
    }

    int width() const
    {
        return width_;
    }

    int height() const
    {
        return height_;
    }

    Vector2I topLeft() const
    {
        return Vector2I{
            left_,
            top_
        };
    }

    Vector2I bottomRight() const
    {
        return Vector2I{
            right_,
            bottom_
        };
    }

    Vector2I center() const
    {
        return Vector2I{
            ( left_ + ( width_ / 2 ) ),
            ( top_ + ( height_ / 2 ) )
        };
    }

    Rectangle rectangle() const
    {
        return Rectangle{
            static_cast<float>( left_ ),
            static_cast<float>( top_ ),
            static_cast<float>( width_ ),
            static_cast<float>( height_ )
        };
    }

private:
    void updateWidth()
    {
        width_ = right_ - left_ + 1;
    }

    void updateHeight()
    {
        height_ = bottom_ - top_ + 1;
    }

    void validate()
    {
        assert( width_ && height_ && "Width or height < 1" );

        if ( right_ < left_ )
        {
            int temp{ right_ };
            right_ = left_;
            left_ = temp;

            updateWidth();
        }

        if ( bottom_ < top_ )
        {
            int temp{ bottom_ };
            bottom_ = top_;
            top_ = temp;

            updateHeight();
        }
    }
};

struct Matrix2x2
{
    float m11, m12;
    float m21, m22;
};

struct Matrix2x2I
{
    int m11, m12;
    int m21, m22;
};

//* Function Overloads

//* Add two vectors (v1 + v2)
RMAPI Vector2I Vector2Add( Vector2I const& v1, Vector2I const& v2 )
{
    return Vector2I{
        v1.x + v2.x,
        v1.y + v2.y
    };
}

//* Subtract two vectors (v1 - v2)
RMAPI Vector2I Vector2Subtract( Vector2I const& v1, Vector2I const& v2 )
{
    return Vector2I{
        v1.x - v2.x,
        v1.y - v2.y
    };
}

//* Scale vector (multiply by value)
RMAPI Vector2 Vector2Scale( Vector2I const& v, float scale )
{
    return Vector2{
        v.x * scale,
        v.y * scale
    };
}

RMAPI Vector2I Vector2Scale( Vector2I const& v, int scale )
{
    return Vector2I{
        v.x * scale,
        v.y * scale
    };
}

//* Check whether two given integer vectors are equal
RMAPI bool Vector2Equals( Vector2I const& v1, Vector2I const& v2 )
{
    return ( ( v1.x == v2.x ) && ( v1.y == v2.y ) );
}

RMAPI Vector2 Vector2Round( Vector2 v )
{
    v.x = roundf( v.x );
    v.y = roundf( v.y );
    return v;
}

RMAPI Vector2I Vector2Negate( Vector2I const& v )
{
    return Vector2I{ -v.x, -v.y };
}

RMAPI Vector2I Vector2Normalize( Vector2I const& v )
{
    return Vector2I{
        ( ( v.x ) ? v.x / abs( v.x ) : v.x ),
        ( ( v.y ) ? v.y / abs( v.y ) : v.y )
    };
}

RMAPI int Vector2Length( Vector2I const& v )
{
    return ( abs( v.x ) + abs( v.y ) );
}

inline bool CheckCollisionPointRec(
    Vector2I const& point,
    RectangleExI const& rec
)
{
    return (
        ( point.x >= rec.left() )
        && ( point.x <= rec.right() )
        && ( point.y >= rec.top() )
        && ( point.y <= rec.bottom() )
    );
}

inline bool CheckCollisionPointRec(
    Vector2 const& point,
    RectangleEx const& rec
)
{
    return CheckCollisionPointRec(
        point,
        rec.rectangle()
    );
}

//* Functions

inline int Vector2Sum( Vector2I const& v )
{
    return abs( v.x ) + abs( v.y );
}

inline Vector2 Vector2MainDirection( Vector2 v )
{
    ( abs( v.x ) > abs( v.y ) )
        ? ( v.y = 0 )
        : ( v.x = 0 );

    return Vector2Normalize( v );
}

inline Vector2I Vector2MainDirection( Vector2I v )
{
    ( abs( v.x ) > abs( v.y ) )
        ? ( v.y = 0 )
        : ( v.x = 0 );

    return Vector2Normalize( v );
}

inline Vector2 Vector2MainDirection( Vector2 const& from, Vector2 const& to )
{
    return Vector2MainDirection(
        Vector2Subtract(
            to,
            from
        )
    );
}

inline Vector2I Vector2MainDirection( Vector2I const& from, Vector2I const& to )
{
    return Vector2MainDirection(
        Vector2Subtract(
            to,
            from
        )
    );
}

inline Vector2 Vector2OffDirection( Vector2 v )
{
    ( abs( v.x ) < abs( v.y ) )
        ? ( v.y = 0 )
        : ( v.x = 0 );

    return Vector2Normalize( v );
}

inline Vector2I Vector2OffDirection( Vector2I v )
{
    ( abs( v.x ) < abs( v.y ) )
        ? ( v.y = 0 )
        : ( v.x = 0 );

    return Vector2Normalize( v );
}

inline Vector2 Vector2OffDirection( Vector2 const& from, Vector2 const& to )
{
    return Vector2OffDirection(
        Vector2Subtract(
            to,
            from
        )
    );
}

inline Vector2I Vector2OffDirection( Vector2I const& from, Vector2I const& to )
{
    return Vector2OffDirection(
        Vector2Subtract(
            to,
            from
        )
    );
}

inline Vector2I Vector2Swap( Vector2I v )
{
    std::swap( v.x, v.y );

    return v;
}

inline Rectangle GetWindowRec()
{
    return Rectangle{
        0,
        0,
        static_cast<float>( GetRenderWidth() ),
        static_cast<float>( GetRenderHeight() )
    };
}

inline void DrawTextExCentered(
    Font font,
    const char* text,
    RectangleEx const& rectangle,
    float yOffsetFromTop,
    float fontSize,
    float spacing,
    Color tint
)
{
    DrawTextEx(
        font,
        text,
        Vector2{
            rectangle.center().x - ( 0.5f * MeasureTextEx( font, text, fontSize, spacing ).x ),
            rectangle.center().y + yOffsetFromTop
        },
        fontSize,
        spacing,
        tint
    );
}

//* Operator Overloads
inline Vector2I& operator+=( Vector2I& lhs, Vector2I const& rhs )
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

inline Vector2I& operator-=( Vector2I& lhs, Vector2I const& rhs )
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

inline bool operator==( Vector2I const& lhs, Vector2I const& rhs )
{
    return ( ( lhs.x == rhs.x ) && ( lhs.y == rhs.y ) );
}

//* Overloads

//* hash
//* https://en.wikipedia.org/wiki/List_of_prime_numbers
int constexpr PRIME{ 2946901 };

template <>
struct std::hash<Vector2>
{
    size_t operator()( Vector2 const& v ) const
    {
        return ( PRIME + std::hash<float>()( v.x ) ) * PRIME + std::hash<float>()( v.y );
    }
};

template <>
struct std::hash<Vector2I>
{
    size_t operator()( Vector2I const& v ) const
    {
        return ( PRIME + std::hash<int>()( v.x ) ) * PRIME + std::hash<int>()( v.y );
    }
};

//* ostream
inline std::ostream& operator<<( std::ostream& os, Vector2 const& v )
{
    return os
           << "("
           << v.x
           << ", "
           << v.y
           << ")";
}

inline std::ostream& operator<<( std::ostream& os, Vector2I const& v )
{
    return os
           << "("
           << v.x
           << ", "
           << v.y
           << ")";
}
#endif
