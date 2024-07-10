#pragma once

#include "matrix/vector.hh"
namespace saltus
{
    enum class SamplerFilter
    {
        Nearest,
        Linear,
    };

    enum class SamplerWraping
    {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
    };

    struct SamplerCreateInfo
    {
        SamplerFilter min_filter = SamplerFilter::Linear;
        SamplerFilter mag_filter = SamplerFilter::Linear;

        SamplerFilter mipmap_mode = SamplerFilter::Linear;

        SamplerWraping wrap_u = SamplerWraping::ClampToEdge;
        SamplerWraping wrap_v = SamplerWraping::ClampToEdge;
        SamplerWraping wrap_w = SamplerWraping::ClampToEdge;

        bool anisotropic_filtering = true;

        matrix::Vector4F border_color = {{ 0., 0., 0., 0. }};
    };

    class Sampler
    {
    public:
        virtual ~Sampler() = 0;

        Sampler(const Sampler&) = delete;
        Sampler(Sampler&&) = delete;
        Sampler& operator=(const Sampler&) = delete;
        Sampler& operator=(Sampler&&) = delete;

        const SamplerFilter &min_filter() const;
        const SamplerFilter &mag_filter() const;

        const SamplerFilter &mipmap_mode() const;

        const SamplerWraping &wrap_u() const;
        const SamplerWraping &wrap_v() const;
        const SamplerWraping &wrap_w() const;

        const bool &anisotropic_filtering() const;

        const matrix::Vector4F &border_color() const;

    protected:
        Sampler(SamplerCreateInfo);

    private:
        SamplerFilter min_filter_;
        SamplerFilter mag_filter_;

        SamplerFilter mipmap_mode_;

        SamplerWraping wrap_u_;
        SamplerWraping wrap_v_;
        SamplerWraping wrap_w_;

        bool anisotropic_filtering_;

        matrix::Vector4F border_color_;
    };
} // namespace saltus
