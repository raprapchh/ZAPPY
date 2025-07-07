#include "utils/Math.hpp"
#include <random>
namespace zappy {
namespace math {
float length(const sf::Vector2f& v) 
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

float lengthSquared(const sf::Vector2f& v) 
{
    return v.x * v.x + v.y * v.y;
}

sf::Vector2f normalize(const sf::Vector2f& v) 
{
    float len = length(v);
    if (len > 0.0f) {
        return sf::Vector2f(v.x / len, v.y / len);
    }
    return sf::Vector2f(0.0f, 0.0f);
}

float dot(const sf::Vector2f& a, const sf::Vector2f& b) 
{
    return a.x * b.x + a.y * b.y;
}

float distance(const sf::Vector2f& a, const sf::Vector2f& b) 
{
    return length(b - a);
}

float distanceSquared(const sf::Vector2f& a, const sf::Vector2f& b) 
{
    return lengthSquared(b - a);
}

float lerp(float a, float b, float t) 
{
    return a + t * (b - a);
}

sf::Vector2f lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t) 
{
    return sf::Vector2f(
        lerp(a.x, b.x, t),
        lerp(a.y, b.y, t)
    );
}

sf::Color lerp(const sf::Color& a, const sf::Color& b, float t) 
{
    return sf::Color(
        static_cast<sf::Uint8>(lerp(static_cast<float>(a.r), static_cast<float>(b.r), t)),
        static_cast<sf::Uint8>(lerp(static_cast<float>(a.g), static_cast<float>(b.g), t)),
        static_cast<sf::Uint8>(lerp(static_cast<float>(a.b), static_cast<float>(b.b), t)),
        static_cast<sf::Uint8>(lerp(static_cast<float>(a.a), static_cast<float>(b.a), t))
    );
}

float smoothstep(float t) 
{
    t = clamp(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

float smootherstep(float t) 
{
    t = clamp(t, 0.0f, 1.0f);
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float easeInOut(float t) 
{
    return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
}

float normalizeAngle(float angle) {
    while (angle > 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}

float angleDifference(float from, float to) 
{
    return normalizeAngle(to - from);
}

float lerpAngle(float from, float to, float t) 
{
    float diff = angleDifference(from, to);
    return from + diff * t;
}

bool pointInRect(const sf::Vector2f& point, const sf::FloatRect& rect) 
{
    return point.x >= rect.left && point.x <= rect.left + rect.width &&
           point.y >= rect.top && point.y <= rect.top + rect.height;
}

bool rectsIntersect(const sf::FloatRect& a, const sf::FloatRect& b) 
{
    return !(a.left + a.width < b.left || b.left + b.width < a.left ||
             a.top + a.height < b.top || b.top + b.height < a.top);
}

sf::FloatRect rectUnion(const sf::FloatRect& a, const sf::FloatRect& b) 
{
    float left = std::min(a.left, b.left);
    float top = std::min(a.top, b.top);
    float right = std::max(a.left + a.width, b.left + b.width);
    float bottom = std::max(a.top + a.height, b.top + b.height);
    return sf::FloatRect(left, top, right - left, bottom - top);
}

sf::FloatRect rectIntersection(const sf::FloatRect& a, const sf::FloatRect& b) 
{
    float left = std::max(a.left, b.left);
    float top = std::max(a.top, b.top);
    float right = std::min(a.left + a.width, b.left + b.width);
    float bottom = std::min(a.top + a.height, b.top + b.height);
    if (right <= left || bottom <= top) {
        return sf::FloatRect(0, 0, 0, 0);
    }
    return sf::FloatRect(left, top, right - left, bottom - top);
}

sf::Vector2f tileToWorld(int tileX, int tileY, float tileSize) 
{
    return sf::Vector2f(
        static_cast<float>(tileX) * tileSize,
        static_cast<float>(tileY) * tileSize
    );
}

sf::Vector2i worldToTile(float worldX, float worldY, float tileSize) 
{
    return sf::Vector2i(
        static_cast<int>(worldX / tileSize),
        static_cast<int>(worldY / tileSize)
    );
}

sf::Vector2f screenToWorld(const sf::Vector2i& screenPos, const sf::View& view) 
{
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();
    sf::Vector2f normalized(
        (2.0f * screenPos.x / viewSize.x) - 1.0f,
        (2.0f * screenPos.y / viewSize.y) - 1.0f
    );
    return viewCenter + sf::Vector2f(
        normalized.x * viewSize.x * 0.5f,
        normalized.y * viewSize.y * 0.5f
    );
}

sf::Vector2i worldToScreen(const sf::Vector2f& worldPos, const sf::View& view,
    const sf::Vector2u& windowSize) 
{
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();
    sf::Vector2f normalized(
        (worldPos.x - viewCenter.x) / (viewSize.x * 0.5f),
        (worldPos.y - viewCenter.y) / (viewSize.y * 0.5f)
    );
    return sf::Vector2i(
        static_cast<int>((normalized.x + 1.0f) * windowSize.x * 0.5f),
        static_cast<int>((normalized.y + 1.0f) * windowSize.y * 0.5f)
    );
}
namespace {
    std::random_device rd;
    std::mt19937 gen(rd());
}

float randomFloat(float min, float max) 
{
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

int randomInt(int min, int max) 
{
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}
sf::Vector2f randomVector2f(float minX, float maxX, float minY, float maxY) 
{
    return sf::Vector2f(randomFloat(minX, maxX), randomFloat(minY, maxY));
}

sf::Color randomColor(sf::Uint8 alpha) 
{
    return sf::Color(
        static_cast<sf::Uint8>(randomInt(0, 255)),
        static_cast<sf::Uint8>(randomInt(0, 255)),
        static_cast<sf::Uint8>(randomInt(0, 255)),
        alpha
    );
}
namespace easing {
    float easeInQuad(float t) 
    {
        return t * t;
    }

    float easeOutQuad(float t) 
    {
        return t * (2.0f - t);
    }

    float easeInOutQuad(float t) 
    {
        return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
    }

    float easeInCubic(float t) 
    {
        return t * t * t;
    }
    float easeOutCubic(float t) 
    {
        float f = t - 1.0f;
        return f * f * f + 1.0f;
    }

    float easeInOutCubic(float t) 
    {
        return t < 0.5f ? 4.0f * t * t * t : (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;
    }

    float easeInSine(float t) 
    {
        return 1.0f - std::cos(t * PI / 2.0f);
    }

    float easeOutSine(float t) 
    {
        return std::sin(t * PI / 2.0f);
    }

    float easeInOutSine(float t) 
    {
        return -(std::cos(PI * t) - 1.0f) / 2.0f;
    }
}
sf::Color hsvToRgb(float h, float s, float v, sf::Uint8 alpha) 
{
    h = wrap(h, 0.0f, 1.0f) * 360.0f;
    s = clamp(s, 0.0f, 1.0f);
    v = clamp(v, 0.0f, 1.0f);
    float c = v * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;
    float r, g, b;
    if (h >= 0 && h < 60) {
        r = c; g = x; b = 0;
    } else if (h >= 60 && h < 120) {
        r = x; g = c; b = 0;
    } else if (h >= 120 && h < 180) {
        r = 0; g = c; b = x;
    } else if (h >= 180 && h < 240) {
        r = 0; g = x; b = c;
    } else if (h >= 240 && h < 300) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }
    return sf::Color(
        static_cast<sf::Uint8>((r + m) * 255),
        static_cast<sf::Uint8>((g + m) * 255),
        static_cast<sf::Uint8>((b + m) * 255),
        alpha
    );
}

void rgbToHsv(const sf::Color& rgb, float& h, float& s, float& v) 
{
    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;
    float maxVal = std::max({r, g, b});
    float minVal = std::min({r, g, b});
    float delta = maxVal - minVal;
    v = maxVal;
    s = (maxVal == 0.0f) ? 0.0f : delta / maxVal;
    if (delta == 0.0f) {
        h = 0.0f;
    } else if (maxVal == r) {
        h = 60.0f * (std::fmod((g - b) / delta, 6.0f));
    } else if (maxVal == g) {
        h = 60.0f * ((b - r) / delta + 2.0f);
    } else {
        h = 60.0f * ((r - g) / delta + 4.0f);
    }
    if (h < 0.0f) h += 360.0f;
    h /= 360.0f;
}

sf::Color adjustBrightness(const sf::Color& color, float factor) 
{
    return sf::Color(
        static_cast<sf::Uint8>(clamp(color.r * factor, 0.0f, 255.0f)),
        static_cast<sf::Uint8>(clamp(color.g * factor, 0.0f, 255.0f)),
        static_cast<sf::Uint8>(clamp(color.b * factor, 0.0f, 255.0f)),
        color.a
    );
}

sf::Color adjustSaturation(const sf::Color& color, float factor) 
{
    float h, s, v;
    rgbToHsv(color, h, s, v);
    s = clamp(s * factor, 0.0f, 1.0f);
    return hsvToRgb(h, s, v, color.a);
}
}
}
