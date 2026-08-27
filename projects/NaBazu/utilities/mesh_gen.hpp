#ifndef NABAZU_MESH_GEN_HPP
#define NABAZU_MESH_GEN_HPP

#include <glm/glm.hpp>

#include <cmath>
#include <vector>

namespace NaBazu
{
    // Builds a flat (non-indexed) position-only triangle list for a unit sphere
    // (radius 0.5, matching the engine's own -0.5..0.5 primitive convention) using
    // latitude/longitude bands. Shared by OrbGlowRenderer (missile) and
    // ExplosionRenderer (each builds its own independent VBO from this data, rather
    // than sharing GL resources across the two effects).
    inline std::vector<float> BuildUnitSphereVertices(int latSegments = 12, int lonSegments = 16)
    {
        std::vector<float> verts;
        verts.reserve(static_cast<std::size_t>(latSegments) * lonSegments * 6 * 3);

        constexpr float kPi = 3.14159265358979323846f;
        const float radius = 0.5f;

        auto pointAt = [&](int lat, int lon) -> glm::vec3
        {
            const float theta = kPi * static_cast<float>(lat) / static_cast<float>(latSegments);
            const float phi = 2.0f * kPi * static_cast<float>(lon) / static_cast<float>(lonSegments);
            return glm::vec3(
                radius * std::sin(theta) * std::cos(phi),
                radius * std::cos(theta),
                radius * std::sin(theta) * std::sin(phi));
        };

        auto push = [&](const glm::vec3 &p)
        {
            verts.push_back(p.x);
            verts.push_back(p.y);
            verts.push_back(p.z);
        };

        for (int lat = 0; lat < latSegments; ++lat)
        {
            for (int lon = 0; lon < lonSegments; ++lon)
            {
                const glm::vec3 p00 = pointAt(lat, lon);
                const glm::vec3 p01 = pointAt(lat, lon + 1);
                const glm::vec3 p10 = pointAt(lat + 1, lon);
                const glm::vec3 p11 = pointAt(lat + 1, lon + 1);

                push(p00); push(p10); push(p11);
                push(p00); push(p11); push(p01);
            }
        }

        return verts;
    }

    // Builds a flat position-only triangle list for a "bolt": two N-sided pyramids
    // joined base-to-base along local Z, tapering to a point at each end -- a pointed
    // dart/energy-bolt silhouette instead of a plain cube. The tip at local -Z is the
    // "forward" point, matching the engine-wide -Z-forward convention (see Ship's nose
    // offset / rail-facing yaw derivation) so callers can reuse the same
    // atan2(-dir.x,-dir.z)/atan2(dir.y,horizLen) formula to face it along travel.
    inline std::vector<float> BuildUnitBoltVertices(int sides = 6)
    {
        std::vector<float> verts;
        verts.reserve(static_cast<std::size_t>(sides) * 6 * 3);

        constexpr float kPi = 3.14159265358979323846f;
        const float halfLength = 0.5f;
        const float radius = 0.22f;

        auto ringPoint = [&](int i) -> glm::vec3
        {
            const float angle = 2.0f * kPi * static_cast<float>(i) / static_cast<float>(sides);
            return glm::vec3(radius * std::cos(angle), radius * std::sin(angle), 0.0f);
        };

        auto push = [&](const glm::vec3 &p)
        {
            verts.push_back(p.x);
            verts.push_back(p.y);
            verts.push_back(p.z);
        };

        const glm::vec3 tipForward(0.0f, 0.0f, -halfLength);
        const glm::vec3 tipBack(0.0f, 0.0f, halfLength);

        for (int i = 0; i < sides; ++i)
        {
            const glm::vec3 a = ringPoint(i);
            const glm::vec3 b = ringPoint((i + 1) % sides);
            push(tipForward); push(b); push(a);
            push(tipBack); push(a); push(b);
        }

        return verts;
    }

    // Builds a position-only triangle list for a torus (ring) lying in the XY plane,
    // with the tube extending along Z. Sized so the outer edge reaches the engine's
    // 0.5 unit-radius convention. Used by the health pickup, which spins around Y so
    // the ring sweeps from a full circle to edge-on and back.
    inline std::vector<float> BuildUnitRingVertices(int majorSegments = 24, int minorSegments = 10, float tubeRatio = 0.22f)
    {
        std::vector<float> verts;
        verts.reserve(static_cast<std::size_t>(majorSegments) * minorSegments * 6 * 3);

        constexpr float kPi = 3.14159265358979323846f;
        const float outerRadius = 0.5f;
        const float tubeRadius = outerRadius * tubeRatio;
        const float ringRadius = outerRadius - tubeRadius;

        auto surfacePoint = [&](int i, int j) -> glm::vec3
        {
            const float u = 2.0f * kPi * static_cast<float>(i) / static_cast<float>(majorSegments);
            const float v = 2.0f * kPi * static_cast<float>(j) / static_cast<float>(minorSegments);
            const float radial = ringRadius + tubeRadius * std::cos(v);
            return glm::vec3(radial * std::cos(u), radial * std::sin(u), tubeRadius * std::sin(v));
        };

        auto push = [&](const glm::vec3 &p)
        {
            verts.push_back(p.x);
            verts.push_back(p.y);
            verts.push_back(p.z);
        };

        for (int i = 0; i < majorSegments; ++i)
        {
            for (int j = 0; j < minorSegments; ++j)
            {
                const glm::vec3 p00 = surfacePoint(i, j);
                const glm::vec3 p10 = surfacePoint(i + 1, j);
                const glm::vec3 p11 = surfacePoint(i + 1, j + 1);
                const glm::vec3 p01 = surfacePoint(i, j + 1);

                push(p00); push(p10); push(p11);
                push(p00); push(p11); push(p01);
            }
        }

        return verts;
    }

    // Appends a position-only triangle list for an axis-aligned box (12 triangles)
    // spanning [minCorner, maxCorner] to an existing vertex buffer.
    inline void AppendBoxTriangles(std::vector<float> &verts, glm::vec3 minCorner, glm::vec3 maxCorner)
    {
        auto push = [&](const glm::vec3 &p)
        {
            verts.push_back(p.x);
            verts.push_back(p.y);
            verts.push_back(p.z);
        };

        const glm::vec3 c000(minCorner.x, minCorner.y, minCorner.z);
        const glm::vec3 c100(maxCorner.x, minCorner.y, minCorner.z);
        const glm::vec3 c010(minCorner.x, maxCorner.y, minCorner.z);
        const glm::vec3 c110(maxCorner.x, maxCorner.y, minCorner.z);
        const glm::vec3 c001(minCorner.x, minCorner.y, maxCorner.z);
        const glm::vec3 c101(maxCorner.x, minCorner.y, maxCorner.z);
        const glm::vec3 c011(minCorner.x, maxCorner.y, maxCorner.z);
        const glm::vec3 c111(maxCorner.x, maxCorner.y, maxCorner.z);

        push(c000); push(c100); push(c110); push(c110); push(c010); push(c000); // -z
        push(c001); push(c101); push(c111); push(c111); push(c011); push(c001); // +z
        push(c000); push(c010); push(c011); push(c011); push(c001); push(c000); // -x
        push(c100); push(c110); push(c111); push(c111); push(c101); push(c100); // +x
        push(c000); push(c100); push(c101); push(c101); push(c001); push(c000); // -y
        push(c010); push(c110); push(c111); push(c111); push(c011); push(c010); // +y
    }

    // Builds a position-only triangle list for an N-pointed star extruded along Z
    // (a flat star given a bit of thickness, so it still reads as a solid object when
    // seen edge-on while spinning). Vertices alternate between outer tips and inner
    // valley points; each of the 2N wedge slices becomes a front face, a back face,
    // and two side quads. Sized to fit the engine's -0.5..0.5 unit convention.
    inline std::vector<float> BuildUnitStarVertices(int points = 5, float innerRatio = 0.42f, float thickness = 0.18f)
    {
        std::vector<float> verts;
        verts.reserve(static_cast<std::size_t>(points) * 2 * 12 * 3);

        constexpr float kPi = 3.14159265358979323846f;
        const float outerRadius = 0.5f;
        const float innerRadius = outerRadius * innerRatio;
        const float halfDepth = thickness * 0.5f;
        const int vertexRingCount = points * 2;

        auto push = [&](const glm::vec3 &p)
        {
            verts.push_back(p.x);
            verts.push_back(p.y);
            verts.push_back(p.z);
        };

        // Ring point i: even -> outer tip, odd -> inner valley. Offset so a tip points
        // straight up (+Y) regardless of the point count.
        auto ringPoint = [&](int i, float z) -> glm::vec3
        {
            const float radius = (i % 2 == 0) ? outerRadius : innerRadius;
            const float angle = kPi * 0.5f + kPi * static_cast<float>(i) / static_cast<float>(points);
            return glm::vec3(radius * std::cos(angle), radius * std::sin(angle), z);
        };

        const glm::vec3 frontCenter(0.0f, 0.0f, halfDepth);
        const glm::vec3 backCenter(0.0f, 0.0f, -halfDepth);

        for (int i = 0; i < vertexRingCount; ++i)
        {
            const int next = (i + 1) % vertexRingCount;

            const glm::vec3 frontA = ringPoint(i, halfDepth);
            const glm::vec3 frontB = ringPoint(next, halfDepth);
            const glm::vec3 backA = ringPoint(i, -halfDepth);
            const glm::vec3 backB = ringPoint(next, -halfDepth);

            // Front and back caps (fan from each face's center).
            push(frontCenter); push(frontA); push(frontB);
            push(backCenter); push(backB); push(backA);

            // Side wall between this ring edge's front and back.
            push(frontA); push(backA); push(backB);
            push(frontA); push(backB); push(frontB);
        }

        return verts;
    }

    // Builds a position-only triangle list for a "crosshair": four short thin bars
    // radiating from the center with a gap in the middle (classic reticle look),
    // each bar a small 3D box (not a flat quad) so it stays visible from any angle
    // rather than disappearing edge-on.
    inline std::vector<float> BuildUnitCrosshairVertices()
    {
        std::vector<float> verts;
        verts.reserve(4 * 6 * 6 * 3);

        const float barLength = 0.34f;
        const float barThickness = 0.07f;
        const float gap = 0.14f;
        const float depth = 0.06f;
        const float halfThick = barThickness * 0.5f;
        const float halfDepth = depth * 0.5f;

        // top
        AppendBoxTriangles(verts, glm::vec3(-halfThick, gap, -halfDepth), glm::vec3(halfThick, gap + barLength, halfDepth));
        // bottom
        AppendBoxTriangles(verts, glm::vec3(-halfThick, -gap - barLength, -halfDepth), glm::vec3(halfThick, -gap, halfDepth));
        // right
        AppendBoxTriangles(verts, glm::vec3(gap, -halfThick, -halfDepth), glm::vec3(gap + barLength, halfThick, halfDepth));
        // left
        AppendBoxTriangles(verts, glm::vec3(-gap - barLength, -halfThick, -halfDepth), glm::vec3(-gap, halfThick, halfDepth));

        return verts;
    }
}

#endif
