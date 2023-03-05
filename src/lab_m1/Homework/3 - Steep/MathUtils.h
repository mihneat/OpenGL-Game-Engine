#pragma once

#include "glm/glm.hpp"

namespace utils
{
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

    private:
        MathUtils() {};
        ~MathUtils() {};

    };
}   // namespace utils
