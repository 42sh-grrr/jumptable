#include "saltus/texture.hh"

namespace saltus
{
    Texture::Texture(TextureCreateInfo info):
        image_(info.image), sampler_(info.sampler)
    { }

    Texture::~Texture()
    { }

    const std::shared_ptr<Image> &Texture::image() const
    {
        return image_;
    }

    const std::shared_ptr<Sampler> &Texture::sampler() const
    {
        return sampler_;
    }
} // namespace saltus
