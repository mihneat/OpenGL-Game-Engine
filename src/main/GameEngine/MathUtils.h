#pragma once

#include <tuple>

#include "glm/glm.hpp"

namespace utils
{
    struct Plane
    {
        // The direction the plane is facing
        glm::vec3 normal;
        // The distance from the origin
        float distance;

        Plane() : normal({0.f, 1.f, 0.f}), distance(0.0f) {}
        Plane(const glm::vec3& point, const glm::vec3& normal): normal(glm::normalize(normal)), distance(glm::dot(glm::normalize(normal), point)) { }

        float GetSignedDistanceToPlane(const glm::vec3& point) const;
    };

    struct AABB
    {
        glm::vec3 center{ 0.f, 0.f, 0.f };
        glm::vec3 extents{ 0.f, 0.f, 0.f };

        AABB(const glm::vec3& center, float extI, float extJ, float extK): center(center), extents({extI, extJ, extK}) { }
        AABB(const glm::vec3& minPoint, const glm::vec3& maxPoint);

        bool IsOnOrInFrontOfPlane(const Plane& plane) const;
    };

    struct Frustum
    {
        Plane topFace;
        Plane bottomFace;

        Plane rightFace;
        Plane leftFace;

        Plane farFace;
        Plane nearFace;
    };
    
    class MathUtils
    {
    public:
        static float GetTriangleArea(glm::vec3 a, glm::vec3 b, glm::vec3 c)
        {
            return glm::length(glm::cross(b - a, c - a)) / 2.0f;
        }

        // NOT TESTED AND NOT USED, i hope it's ok :c
        // Source: http://eecs.qmul.ac.uk/~gslabaugh/publications/euler.pdf
        static glm::vec3 RotationMatrixToEulerAngles(glm::mat4 rotationMatrix)
        {
            float oX, oY, oZ;
            oY = -glm::asin(rotationMatrix[3][1]);
            oX = glm::atan(rotationMatrix[3][2] / glm::cos(oX), rotationMatrix[3][3] / glm::cos(oX));
            oZ = glm::atan(rotationMatrix[2][1] / glm::cos(oX), rotationMatrix[1][1] / glm::cos(oX));

            return glm::vec3(oX, oY, oZ);
        }

        // Source: https://stackoverflow.com/questions/21622956/how-to-convert-direction-vector-to-euler-angles
        static glm::vec3 NormalizedVectorsToEulerAngles(glm::vec3 forward, glm::vec3 up)
        {
            forward = glm::normalize(forward);
            up = glm::normalize(up);

            float angle_H = glm::atan(forward.y, forward.x);
            float angle_P = glm::asin(forward.z);

            glm::vec3 W0 = glm::vec3(-forward.y, forward.x, 0.0f);
            glm::vec3 U0 = glm::cross(W0, forward);
            float angle_B = glm::atan(glm::dot(W0, up) / glm::length(W0), glm::dot(U0, up) / glm::length(U0));

            return glm::vec3(angle_P, angle_H, angle_B);
        }

        // Thanks to: https://www.youtube.com/watch?v=HC5YikQxwZA
        // The generalized formulas were done on paper, by hand
        static std::tuple<bool, bool, glm::vec3, glm::vec3> ClosestPointsBetweenLines(glm::vec3 p1, glm::vec3 v1, glm::vec3 p2, glm::vec3 v2)
        {
            // TODO: What should be done if v1 and v2 are orthogonal? kc would be 0, and the hit point would be NaN..
            // This might not happen in the edge-edge collision check (it did when it had a mistake), but could happen to others
            
            glm::vec3 v1_n = glm::normalize(v1);
            glm::vec3 v2_n = glm::normalize(v2);
            
            // General case
            float kc = glm::dot(v1, v2);
            float kt1 = glm::dot(v1, v1);
            float ks2 = glm::dot(v2, v2);
            float k1 = glm::dot(v1, p2 - p1);
            float k2 = glm::dot(v2, p2 - p1);

            float t;
            if (glm::dot(v1_n, v2_n) > 0.999999f)
                // Directions are parallel, choose a random t
                t = 0.5f;
            else
                // General case, a single closest point
                t = (k1 * ks2 - k2 * kc) / (ks2 * kt1 - kc * kc);
            
            float s = (t * kt1 - k1) / kc;

            bool pOnSegment = t >= 0.0f && t <= 1.0f;
            bool qOnSegment = s >= 0.0f && s <= 1.0f;

            return { pOnSegment, qOnSegment, p1 + t * v1, p2 + s * v2 };
        }

    private:
        MathUtils() {}
        ~MathUtils() {}

    };
}   // namespace utils
