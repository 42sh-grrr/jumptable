#pragma once

#include <vector>

namespace saltus
{
    struct ShaderCreateInfo
    {
        std::vector<char> source_code_;
    };

    class Shader
    {
    public:
        virtual ~Shader() = default;

    protected:
        Shader() = default;

    private:
    };
} // namespace saltus
