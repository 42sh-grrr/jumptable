#pragma once

#include <vector>

namespace saltus
{
    enum class ShaderKind
    {
        Vertex,
        Fragment,
    };

    struct ShaderCreateInfo
    {
        ShaderKind kind;

        std::vector<char> source_code;
    };

    class Shader
    {
    public:
        virtual ~Shader() = 0;
        Shader(const Shader &x) = delete;
        const Shader &operator =(const Shader &x) = delete;

        ShaderKind kind() const;

    protected:
        Shader(ShaderCreateInfo info);

    private:
        ShaderKind kind_;
    };
} // namespace saltus
