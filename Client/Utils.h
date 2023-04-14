#pragma once
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include "ocular/imgui/imgui.h"

struct Vector2 {
    union {
        struct {
            float x;
            float y;
        };

        float v[2];
    };
};


struct Vector3 {
    union {
        struct {
            float x;
            float y;
            float z;
        };

        float v[3];
    };

    Vector3() { x = 0.0f; y = 0.0f; z = 0.0f; }

    Vector3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }

    Vector3(float v[3]) { x = v[0]; y = v[1]; z = v[2]; }

    Vector3(std::tuple<float, float, float> tuple) { x = std::get<0>(tuple); y = std::get<1>(tuple); z = std::get<2>(tuple); }

    inline bool operator == (Vector3 other) {
        return (x == other.x && y == other.y && z == other.z);
    }

    inline Vector3 operator = (float other[3]) {
        x = other[0];
        y = other[1];
        z = other[2];
        return *this;
    }

    inline Vector3 operator = (std::tuple<float, float, float> other) {
        x = std::get<0>(other);
        y = std::get<1>(other);
        z = std::get<2>(other);
        return *this;
    }

    inline Vector3 operator + (Vector3 other) {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    inline Vector3 operator - (Vector3 other) {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    inline Vector3 operator * (float other) {
        return Vector3(x * other, y * other, z * other);
    }

    inline float DistanceTo(Vector3 other) {
        return sqrtf(powf(other.x - x, 2) + powf(other.y - y, 2) + powf(other.z - z, 2));
        //return sqrt(pow(other.x - x, 2) + pow(other.y - y, 2) + pow(other.z - z, 2));
    }

    inline Vector3 LerpTo(Vector3 other, float perc) {
        return Vector3(*this + (other - *this) * perc);
    }

};

struct Vector4 {
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };

        float v[4];
    };

    Vector4() { x = 0.0f; y = 0.0f; z = 0.0f; w = 0.0f; }

    Vector4(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }

    Vector4(float v[4]) { x = v[0]; y = v[1]; z = v[2]; w = v[3]; }

    Vector4(std::tuple<float, float, float, float> tuple) { x = std::get<0>(tuple); y = std::get<1>(tuple); z = std::get<2>(tuple); w = std::get<3>(tuple);
    }

    inline bool operator == (Vector4 other) {
        return (x == other.x && y == other.y && z == other.z && w == other.w);
    }

    inline Vector4 operator = (float other[4]) {
        x = other[0];
        y = other[1];
        z = other[2];
        w = other[3];
        return *this;
    }

    inline Vector4 operator = (std::tuple<float, float, float, float> other) {
        x = std::get<0>(other);
        y = std::get<1>(other);
        z = std::get<2>(other);
        w = std::get<3>(other);
        return *this;
    }

    inline Vector4 operator + (Vector4 other) {
        return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    inline Vector4 operator - (Vector4 other) {
        return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    inline Vector4 operator * (float other) {
        return Vector4(x * other, y * other, z * other, w * other);
    }

    inline float DistanceTo(Vector4 other) {
        return sqrtf(powf(other.x - x, 2) + powf(other.y - y, 2) + powf(other.z - z, 2) + powf(other.w - w, 2));
    }

    inline Vector4 LerpTo(Vector4 other, float perc) {
        return Vector4(*this + (other - *this) * perc);
    }

};

class Matrix4 {
public:
    union
    {
        struct
        {
            float _00, _01, _02, _03;
            float _10, _11, _12, _13;
            float _20, _21, _22, _23;
            float _30, _31, _32, _33;
        };

        float m[4][4];
        float mm[16];
    };

    inline Matrix4 Transpose();
    inline Matrix4& operator*(const Matrix4& matrix);
};

inline Matrix4& Matrix4::operator*(const Matrix4& matrix)
{
    m[0][0] = m[0][0] * matrix.m[0][0] + m[1][0] * matrix.m[0][1] + m[2][0] * matrix.m[0][2] + m[3][0] * matrix.m[0][3];
    m[0][1] = m[0][1] * matrix.m[0][0] + m[1][1] * matrix.m[0][1] + m[2][1] * matrix.m[0][2] + m[3][1] * matrix.m[0][3];
    m[0][2] = m[0][2] * matrix.m[0][0] + m[1][2] * matrix.m[0][1] + m[2][2] * matrix.m[0][2] + m[3][2] * matrix.m[0][3];
    m[0][3] = m[0][3] * matrix.m[0][0] + m[1][3] * matrix.m[0][1] + m[2][3] * matrix.m[0][2] + m[3][3] * matrix.m[0][3];

    m[1][0] = m[0][0] * matrix.m[1][0] + m[1][0] * matrix.m[1][1] + m[2][0] * matrix.m[1][2] + m[3][0] * matrix.m[1][3];
    m[1][1] = m[0][1] * matrix.m[1][0] + m[1][1] * matrix.m[1][1] + m[2][1] * matrix.m[1][2] + m[3][1] * matrix.m[1][3];
    m[1][2] = m[0][2] * matrix.m[1][0] + m[1][2] * matrix.m[1][1] + m[2][2] * matrix.m[1][2] + m[3][2] * matrix.m[1][3];
    m[1][3] = m[0][3] * matrix.m[1][0] + m[1][3] * matrix.m[1][1] + m[2][3] * matrix.m[1][2] + m[3][3] * matrix.m[1][3];

    m[2][0] = m[0][0] * matrix.m[2][0] + m[1][0] * matrix.m[2][1] + m[2][0] * matrix.m[2][2] + m[3][0] * matrix.m[2][3];
    m[2][1] = m[0][1] * matrix.m[2][0] + m[1][1] * matrix.m[2][1] + m[2][1] * matrix.m[2][2] + m[3][1] * matrix.m[2][3];
    m[2][2] = m[0][2] * matrix.m[2][0] + m[1][2] * matrix.m[2][1] + m[2][2] * matrix.m[2][2] + m[3][2] * matrix.m[2][3];
    m[2][3] = m[0][3] * matrix.m[2][0] + m[1][3] * matrix.m[2][1] + m[2][3] * matrix.m[2][2] + m[3][3] * matrix.m[2][3];

    m[3][0] = m[0][0] * matrix.m[3][0] + m[1][0] * matrix.m[3][1] + m[2][0] * matrix.m[3][2] + m[3][0] * matrix.m[3][3];
    m[3][1] = m[0][1] * matrix.m[3][0] + m[1][1] * matrix.m[3][1] + m[2][1] * matrix.m[3][2] + m[3][1] * matrix.m[3][3];
    m[3][2] = m[0][2] * matrix.m[3][0] + m[1][2] * matrix.m[3][1] + m[2][2] * matrix.m[3][2] + m[3][2] * matrix.m[3][3];
    m[3][3] = m[0][3] * matrix.m[3][0] + m[1][3] * matrix.m[3][1] + m[2][3] * matrix.m[3][2] + m[3][3] * matrix.m[3][3];

    return *this;
}

inline bool DXWorldToScreen(Matrix4* vMatrix, Vector3& position, int scrnWidth, int scrnHeight, Vector2& scrnPosOut) {
    Matrix4 tVMatrix = (*vMatrix).Transpose();
    float clip_x = position.x * tVMatrix.mm[0] + position.y * tVMatrix.mm[1] + position.z * tVMatrix.mm[2] + tVMatrix.mm[3];
    float clip_y = position.x * tVMatrix.mm[4] + position.y * tVMatrix.mm[5] + position.z * tVMatrix.mm[6] + tVMatrix.mm[7];
    float clip_z = position.x * tVMatrix.mm[8] + position.y * tVMatrix.mm[9] + position.z * tVMatrix.mm[10] + tVMatrix.mm[11];
    float clip_w = position.x * tVMatrix.mm[12] + position.y * tVMatrix.mm[13] + position.z * tVMatrix.mm[14] + tVMatrix.mm[15];

    if (clip_w < 0.01f)
        return false;

    Vector3 NDC;
    NDC.x = clip_x / clip_w;
    NDC.y = clip_y / clip_w;
    NDC.z = clip_z / clip_w;

    scrnPosOut.x = (scrnWidth / 2 * NDC.x) + (NDC.x + scrnWidth / 2);
    scrnPosOut.y = -(scrnHeight / 2 * NDC.y) + (NDC.y + scrnHeight / 2);
    return true;
}

inline bool GLWorldToScreen(Matrix4* vMatrix, float* fov, Vector3& position, int scrnWidth, int scrnHeight, Vector2& scrnPosOut) {
    float nearPlane = 1.0f;
    float farPlane = 100.f;
    float aspect = (float)scrnWidth / scrnHeight;

    glm::mat4 projection = glm::perspective((*fov) * -1.0f, aspect, nearPlane, farPlane);
    glm::mat4* model = reinterpret_cast<glm::mat4*>(vMatrix);
   
    auto res = glm::project(glm::vec3{position.x, position.y, position.z}, *model, projection, glm::vec4{ 0.0f, 0.0f, scrnWidth, scrnHeight });
    scrnPosOut.x = res.x;
    scrnPosOut.y = res.y;
    return true;
}

inline Matrix4 Matrix4::Transpose()
{
    Matrix4 transposed;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            transposed.m[j][i] = m[i][j];
        }
    }
    return transposed;
}

inline ImColor Float3ToImColor(float array[3]) {
    return ImColor(array[0], array[1], array[2], 1.0f);
}

inline ImColor Float3AToImColor(float array[3], float alpha) {
    return ImColor(array[0], array[1], array[2], alpha);
}

inline ImColor Float4ToImColor(float array[4]) {
    return ImColor(array[0], array[1], array[2], array[3]);
}