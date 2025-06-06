#ifndef IG20240315204900
#define IG20240315204900

//* Included as an example of a parameterized macro
#define DISALLOW_COPY_AND_ASSIGN( T ) \
    T( T const& ) = delete;           \
    T& operator=( T const& ) = delete;

namespace snx
{
    template <typename Type>
    class Singleton
    {
    public:
        static Type& instance()
        {
            static Type instance;
            return instance;
        }

    protected:
        Singleton() = default;
        virtual ~Singleton() = default;

        Singleton( Singleton const& ) = delete;
        Singleton& operator=( Singleton const& ) = delete;
    };
}

#endif
