////////////////////////////////////////////////////////////
//
// The MIT License (MIT)
//
// STP - SFML TMX Parser
// Copyright (c) 2013-2014 Manuel Sabogal
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
////////////////////////////////////////////////////////////

#include "STP/Core/Layer.hpp"

#include <string>
#include <vector>

#include "SFML/Graphics/RenderTarget.hpp"

namespace tmx {

Layer::Layer() {}

Layer::Layer(const std::string& name, unsigned int width,
             unsigned int height, float opacity, bool visible) :
        MapObject(name, width, height, opacity, visible) {
    // Reserve space for each vector to avoid reallocate
    tiles_.reserve(width * height);
    unsigned char alpha = static_cast<unsigned char>(255 * opacity);
    color_.a = alpha;
}

void Layer::AddTile(tmx::Layer::Tile&& newtile) {
    newtile.SetColor(color_);
    tiles_.push_back(std::move((newtile)));
}

void Layer::SetOpacity(float opacity) {
    color_.a = static_cast<unsigned char>(255 * opacity);
    for (auto& tile : tiles_) {
        tile.SetColor(color_);
    }
}

void Layer::SetColor(const sf::Color& color) {
    color_.r = color.r;
    color_.g = color.g;
    color_.b = color.b;
    for (auto& tile : tiles_) {
        tile.SetColor(color_);
    }
}

void Layer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (visible) {
        for (unsigned int i = 0; i < tiles_.size(); ++i)
            target.draw(tiles_[i]);
    }
}

////////////////////////////////////////////////////////////
// Layer::Tile implementation
////////////////////////////////////////////////////////////

Layer::Tile::Tile() {}

Layer::Tile::Tile(unsigned int gid, sf::IntRect tile_rect,
                  const sf::Texture* texture, sf::IntRect texture_rect) :
        gid_(gid),
        tile_rect_(tile_rect),
        texture_(texture),
        texture_rect_(texture_rect) {
    UpdatePositions();
    UpdateTexCoords();
}

void Layer::Tile::UpdatePositions() {
    sf::FloatRect bounds = GetGlobalBounds();

    vertices_[0].position = sf::Vector2f(bounds.left, bounds.top);
    vertices_[1].position = sf::Vector2f(bounds.left + bounds.width, bounds.top);
    vertices_[2].position = sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height);
    vertices_[3].position = sf::Vector2f(bounds.left, bounds.top + bounds.height);
}

void Layer::Tile::UpdateTexCoords() {
    float left   = static_cast<float>(texture_rect_.left);
    float right  = left + texture_rect_.width;
    float top    = static_cast<float>(texture_rect_.top);
    float bottom = top + texture_rect_.height;

    vertices_[0].texCoords = sf::Vector2f(left, top);
    vertices_[1].texCoords = sf::Vector2f(right, top);
    vertices_[2].texCoords = sf::Vector2f(right, bottom);
    vertices_[3].texCoords = sf::Vector2f(left, bottom);
}

sf::FloatRect Layer::Tile::GetGlobalBounds() const {
    float left = static_cast<float>(tile_rect_.left);
    float top = static_cast<float>(tile_rect_.top);
    float width = static_cast<float>(tile_rect_.width);
    float height = static_cast<float>(tile_rect_.height);

    return sf::FloatRect(left, top, width, height);
}

void Layer::Tile::SetColor(const sf::Color& color) {
    vertices_[0].color = color;
    vertices_[1].color = color;
    vertices_[2].color = color;
    vertices_[3].color = color;
}

void Layer::Tile::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (texture_) {
        states.texture = texture_;
        target.draw(vertices_, 4, sf::Quads, states);
    }
}

}  // namespace tmx