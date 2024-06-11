#include "saltus/shader.hh"

namespace saltus
{
    Shader::Shader(ShaderCreateInfo info)
        : kind_(info.kind)
    { }

    Shader::~Shader()
    { }

    ShaderKind Shader::kind() const
    {
        return kind_;
    }
} // namespace saltus
