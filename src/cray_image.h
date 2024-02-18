#pragma once

#include <string>
#include "stb_image.h"
#include "common.h"

namespace cray {

class CRayImage {
public:
    CRayImage() : data_(nullptr) {}
    CRayImage(const std::string& path) {
        data_ = stbi_load(path.c_str(), &width_, &height_, &nr_components_,
                          bytes_per_pixel);
        bytes_per_scanline_ = width_ * bytes_per_pixel;
    }
    ~CRayImage() { stbi_image_free(data_); }

    bool invalid() const { return data_ == nullptr; }

    int width() const { return data_ == nullptr ? 0 : width_; }
    int height() const { return data_ == nullptr ? 0 : height_; }

    const unsigned char* pixel_data(int x, int y) const {
        if (data_ == nullptr) return Magenta;

        x = clamp(x, 0, width_);
        y = clamp(y, 0, height_);

        return data_ + y * bytes_per_scanline_ + x * bytes_per_pixel;
    }

private:
    unsigned char* data_;
    int width_, height_, nr_components_;
    int bytes_per_scanline_;

    const int bytes_per_pixel = 3;
};

}  // namespace cray