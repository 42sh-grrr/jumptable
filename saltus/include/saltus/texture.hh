#pragma once

#include <memory>
#include "saltus/image.hh"
#include "saltus/sampler.hh"

namespace saltus
{
    struct TextureCreateInfo
    {
        std::shared_ptr<Image> image;
        std::shared_ptr<Sampler> sampler;
    };

    class Texture
    {
    public:
        virtual ~Texture() = 0;

        Texture(const Texture&) = delete;
        Texture(Texture&&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture& operator=(Texture&&) = delete;

        const std::shared_ptr<Image> &image() const;
        const std::shared_ptr<Sampler> &sampler() const;

    protected:
        Texture(TextureCreateInfo);

    private:
        std::shared_ptr<Image> image_;
        std::shared_ptr<Sampler> sampler_;
    };
} // namespace saltus
