#pragma once

namespace component
{
    class IResetable
    {
    public:
        IResetable();
        ~IResetable();

        virtual void Reset() = 0;

    protected:
    };
}
