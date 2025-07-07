#pragma once
#include "Common.hpp"
#include <cmath>
namespace zappy {
namespace math {
#ifndef PI
constexpr float PI = 3.14159265358979323846f;
#endif
constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / PI;

float length(const sf::Vector2f& v);
float lengthSquared(const sf::Vector2f& v);
sf::Vector2f normalize(const sf::Vector2f& v);
float dot(const sf::Vector2f& a, const sf::Vector2f& b);
float distance(const sf::Vector2f& a, const sf::Vector2f& b);
float distanceSquared(const sf::Vector2f& a, const sf::Vector2f& b);

float lerp(float a, float b, float t);
sf::Vector2f lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t);
sf::Color lerp(const sf::Color& a, const sf::Color& b, float t);

float smoothstep(float t);
float smootherstep(float t);
float easeInOut(float t);

float normalizeAngle(float angle);
float angleDifference(float from, float to);
float lerpAngle(float from, float to, float t);

bool pointInRect(const sf::Vector2f& point, const sf::FloatRect& rect);
bool rectsIntersect(const sf::FloatRect& a, const sf::FloatRect& b);
sf::FloatRect rectUnion(const sf::FloatRect& a, const sf::FloatRect& b);
sf::FloatRect rectIntersection(const sf::FloatRect& a, const sf::FloatRect& b);

sf::Vector2f tileToWorld(int tileX, int tileY, float tileSize = 32.0f);
sf::Vector2i worldToTile(float worldX, float worldY, float tileSize = 32.0f);
sf::Vector2f screenToWorld(const sf::Vector2i& screenPos, const sf::View& view);
sf::Vector2i worldToScreen(const sf::Vector2f& worldPos, const sf::View& view,
                           const sf::Vector2u& windowSize);

template<typename T>
T clamp(T value, T min, T max) {
    return std::max(min, std::min(max, value));
}

template<typename T>
T wrap(T value, T min, T max) {
    T range = max - min;
    if (range <= 0) return min;
    while (value < min) value += range;
    while (value >= max) value -= range;
    return value;
}

float randomFloat(float min = 0.0f, float max = 1.0f);
int randomInt(int min, int max);
sf::Vector2f randomVector2f(float minX, float maxX, float minY, float maxY);
sf::Color randomColor(sf::Uint8 alpha = 255);

namespace easing {
    float easeInQuad(float t);
    float easeOutQuad(float t);
    float easeInOutQuad(float t);
    float easeInCubic(float t);
    float easeOutCubic(float t);
    float easeInOutCubic(float t);
    float easeInSine(float t);
    float easeOutSine(float t);
    float easeInOutSine(float t);
}

sf::Color hsvToRgb(float h, float s, float v, sf::Uint8 alpha = 255);
void rgbToHsv(const sf::Color& rgb, float& h, float& s, float& v);
sf::Color adjustBrightness(const sf::Color& color, float factor);
sf::Color adjustSaturation(const sf::Color& color, float factor);
}
}
