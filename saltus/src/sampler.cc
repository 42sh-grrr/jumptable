#include "saltus/sampler.hh"

namespace saltus
{
    Sampler::Sampler(SamplerCreateInfo info):
        min_filter_(info.min_filter),
        mag_filter_(info.mag_filter),
        mipmap_mode_(info.mipmap_mode),
        wrap_u_(info.wrap_u),
        wrap_v_(info.wrap_v),
        wrap_w_(info.wrap_w),
        anisotropic_filtering_(info.anisotropic_filtering),
        border_color_(info.border_color)
    { }

    Sampler::~Sampler()
    { }

    const SamplerFilter &Sampler::min_filter() const
    {
        return min_filter_;
    }

    const SamplerFilter &Sampler::mag_filter() const
    {
        return mag_filter_;
    }

    const SamplerFilter &Sampler::mipmap_mode() const
    {
        return mipmap_mode_;
    }

    const SamplerWraping &Sampler::wrap_u() const
    {
        return wrap_u_;
    }

    const SamplerWraping &Sampler::wrap_v() const
    {
        return wrap_v_;
    }

    const SamplerWraping &Sampler::wrap_w() const
    {
        return wrap_w_;
    }

    const bool &Sampler::anisotropic_filtering() const
    {
        return anisotropic_filtering_;
    }

    const matrix::Vector4F &Sampler::border_color() const
    {
        return border_color_;
    }
}
