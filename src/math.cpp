#include "math/math.h"
#include "core/log.h"

namespace Rainy
{
    bool InterRayAndSurface(Vector3f surfPoint,
                            Vector3f surfNormal,
                            Vector3f rayStartPoint,
                            Vector3f rayDir,
                            Vector3f &dest,
                            float &rayScale)
    {
        float denominator = -(surfNormal.dot(rayDir));

        if (denominator != 0)
        {
            float numerator = surfNormal.dot(rayStartPoint - surfPoint);
            rayScale = numerator / denominator;
            dest = rayStartPoint + rayDir * rayScale;

            if (rayScale < 0)
                return false;

            return true;
        }
        return false;
    }

    bool InterRayAndSphere(Vector3f sphereCenter,
                           float radius,
                           Vector3f rayStartPoint,
                           Vector3f rayDir,
                           Vector3f &dest,
                           float &rayScale)
    {
        Vector3f v = rayStartPoint - sphereCenter;
        float b = rayDir.dot(v);
        float c = v.lengthQuad() - radius * radius;
        float D = b * b - c;

        if (D < 0)
            return false;

        float t = 0;
        if (D > 0)
        {
            t = ((-b) - std::sqrt(D));
            float val = ((-b) + std::sqrt(D));
            if (t > val)
                t = val;
        }
        else
            t = -b;

        dest = rayStartPoint + rayDir * t;
        rayScale = t;
        return true;
    }

    Vector3f BarycentricCoord(Vector3f const &p,
                              Vector3f const &p0,
                              Vector3f const &p1,
                              Vector3f const &p2)
    {
        float det = (p1.z - p2.z) * (p0.x - p2.x) + (p2.x - p1.x) * (p0.z - p2.z);
        float w0 = ((p1.z - p2.z) * (p.x - p2.x) + (p2.x - p1.x) * (p.z - p2.z)) / det;
        float w1 = ((p2.z - p0.z) * (p.x - p2.x) + (p0.x - p2.x) * (p.z - p2.z)) / det;
        float w2 = 1 - w0 - w1;
        return { w0, w1, w2 };
    }

    Matrix4f CreateModelMatrix(Vector3f const &position,
                               Vector3f const &rotation,
                               float const &Scale)
    {
        Matrix4f matrix;
        matrix.Translate(position);
        matrix.Rotate(rotation.x, Vector3f{ 1, 0, 0 });
        matrix.Rotate(rotation.y, Vector3f{ 0, 1, 0 });
        matrix.Rotate(rotation.z, Vector3f{ 0, 0, 1 });
        matrix.Scale(Scale);
        return matrix;
    };

    Matrix4f CreateModelMatrix(Vector3f const &position,
                               Vector3f const &rotation,
                               Vector3f const &scale)
    {
        Matrix4f matrix;
        matrix.Translate(position);
        matrix.Rotate(rotation.x, Vector3f{ 1, 0, 0 });
        matrix.Rotate(rotation.y, Vector3f{ 0, 1, 0 });
        matrix.Rotate(rotation.z, Vector3f{ 0, 0, 1 });
        matrix.Scale({ scale, 1 });
        return matrix;
    }

    Matrix4f CreateViewMatrix(Vector3f const &pos, float const &x_rot, float const &y_rot)
    {
        Matrix4f matrix;
        matrix.Rotate(x_rot, Vector3f{ 1, 0, 0 });
        matrix.Rotate(-y_rot, Vector3f{ 0, 1, 0 });
        matrix.Translate(Vector3f{ -pos.x, -pos.y, -pos.z });
        return matrix;
    }

    Matrix4f CreateLookAtMatrix(Vector3f pos, Vector3f at, Vector3f up)
    {
        Vector3f zAxis = (at - pos).normalize();
        Vector3f xAxis = up.cross(zAxis).normalize();
        Vector3f yAxis = zAxis.cross(xAxis);
        Matrix4f matrix = {
            { xAxis.x, xAxis.y, xAxis.z, xAxis.dot(pos * (-1.f)) },
            { yAxis.x, yAxis.y, yAxis.z, yAxis.dot(pos * (-1.f)) },
            { zAxis.x, zAxis.y, zAxis.z, zAxis.dot(pos * (-1.f)) },
            { 0, 0, 0, 1 },
        };
        return matrix;
    }

    Matrix4f CreatePerspectiveMatrix(float const &ar,
                                     float const &fov,
                                     float const &nearPoint,
                                     float const &farPoint)
    {
        using std::tan;

        Matrix4f matrix;

        float radAngle = ToRadian(fov / 2.f);
        float tanRes = tanf(radAngle);
        float frustrumLength = farPoint - nearPoint;
        matrix[0][0] = (1.f / (tanRes * ar));
        matrix[1][1] = (1.f / tanRes);
        matrix[2][2] = -(farPoint + nearPoint) / frustrumLength;
        matrix[2][3] = -(2 * farPoint * nearPoint) / frustrumLength;
        matrix[3][2] = -1;
        matrix[3][3] = 0;

        return matrix;

        // float tangent = tanf(ToRadian(fov / 2));	// tangent of half fovY
        // float height = nearPoint * tangent;			// half height of near plane
        // float width = height * ar;					// half width of near plane
        // return CreatePerspectiveMatrix(-width, width, -height, height,
        //	nearPoint, farPoint);
    }

    Matrix4f CreatePerspectiveMatrix(float left,
                                     float right,
                                     float bottom,
                                     float top,
                                     float nearPoint,
                                     float farPoint)
    {
        Matrix4f matrix;

        matrix[0][0] = 2 * nearPoint / (right - left);
        matrix[1][1] = 2 * nearPoint / (top - bottom);
        matrix[0][2] = (right + left) / (right - left);
        matrix[1][2] = (top + bottom) / (top - bottom);
        matrix[2][2] = (farPoint + nearPoint) / (farPoint - nearPoint);
        matrix[2][3] = -2 * farPoint * nearPoint / (farPoint - nearPoint);
        matrix[3][2] = 1;
        matrix[3][3] = 0;

        return matrix;
    }

    Matrix4f CreateOrthographicMatrix(float left,
                                      float right,
                                      float bottom,
                                      float top,
                                      float nearPoint,
                                      float farPoint)
    {
        Matrix4f matrix;

        matrix[0][0] = 2 / (right - left);
        matrix[1][1] = 2 / (top - bottom);
        matrix[2][2] = 2 / (farPoint - nearPoint);
        matrix[0][3] = -(right + left) / (right - left);
        matrix[1][3] = -(top + bottom) / (top - bottom);
        matrix[2][3] = -(farPoint + nearPoint) / (farPoint - nearPoint);

        return matrix;
    }

    Vector3f GetLookDirection(Vector3f rotation)
    {
        using std::cos;
        using std::sin;

        float xRotRad = ToRadian(rotation.x);
        float yRotRad = ToRadian(rotation.y);

        return { sin(yRotRad) * cos(xRotRad), sin(xRotRad), cos(yRotRad) * cos(xRotRad) };
    }

    float ToRadian(float angle) { return angle * (PI / 180); }

    Vector3f ViewportSpaceToWorldSpace(Vector2f const &viewport_position,
                                       Matrix4f const &projection_matrix,
                                       Matrix4f const &view_matrix,
                                       uint16_t viewport_width,
                                       uint16_t viewport_height)
    {
        Vector4f clipSpaceV = { (viewport_position.x / viewport_width) * 2 - 1,
                                -((viewport_position.y / viewport_height) * 2 - 1),
                                1,
                                1 };

        Matrix4f invProjM = projection_matrix.Inverse();
        Matrix4f invViewM = view_matrix.Inverse();

        Vector4f eyeSpaceV = invProjM.Mul(clipSpaceV);
        eyeSpaceV.w = 0;
        Vector4f worldSpaceV = invViewM.Mul(eyeSpaceV);

        return worldSpaceV.getVector3();
    }

    /*
    bool RectangleAreasOverlay(RectangleAreai first, RectangleAreai second)
    {
        if (first.LeftBorder < second.RightBorder)
            return true;
        if (first.RightBorder > second.LeftBorder)
            return true;
        if (first.BottomBorder < second.TopBorder)
            return true;
        if (first.TopBorder > second.BottomBorder)
            return true;
        return false;
    }

    bool RectangleAreasOverlay(RectangleAreaf first, RectangleAreaf second)
    {
        if (first.LeftBorder < second.RightBorder)
            return true;
        if (first.RightBorder > second.LeftBorder)
            return true;
        if (first.BottomBorder < second.TopBorder)
            return true;
        if (first.TopBorder > second.BottomBorder)
            return true;
        return false;
    }
bool RectangleAreasOverlay(RectangleAreai first, RectangleAreai second, RectangleAreai & dest)
    {
        bool overley = false;
        if (first.LeftBorder < second.RightBorder) {
            overley = true;
            dest.LeftBorder = first.LeftBorder;
        }
        if (first.RightBorder > second.LeftBorder) {
            overley = true;
            dest.RightBorder = first.RightBorder;
        }
        if (first.BottomBorder < second.TopBorder) {
            overley = true;
            dest.BottomBorder = first.BottomBorder;
        }
        if (first.TopBorder > second.BottomBorder) {
            overley = true;
            dest.TopBorder = first.TopBorder;
        }

        if (dest.BottomBorder > dest.TopBorder)
            std::swap(dest.BottomBorder, dest.TopBorder);
        if (dest.LeftBorder > dest.RightBorder)
            std::swap(dest.BottomBorder, dest.TopBorder);

        return false;
    }
*/

    float Centerf(float min, float max) { return (max - min) / 2.f + min; }

    bool AABB2DOverlay(AABB2Df first, AABB2Df second, AABB2Df &dest)
    {
        auto moveArea = [](Rainy::AABB2Df &area, float xOffset, float yOffset)
        {
            area.Min.x += xOffset;
            area.Max.x += xOffset;
            area.Min.y += yOffset;
            area.Max.y += yOffset;
        };

        float xOffset = std::fmaxf(std::abs(first.Min.x), std::abs(second.Min.x));
        float yOffset = std::fmaxf(std::abs(first.Min.y), std::abs(second.Min.y));

        moveArea(first, xOffset, yOffset);
        moveArea(second, xOffset, yOffset);

        using std::fmaxf;
        using std::fminf;

        Rainy::AABB2Df overlay = {
            { fmaxf(first.Min.x, second.Min.x), fmaxf(first.Min.y, second.Min.y) },
            { fminf(first.Max.x, second.Max.x), fminf(first.Max.y, second.Max.y) }
        };

        if (overlay.Max.x - overlay.Min.x < 0 || overlay.Max.y - overlay.Min.x < 0)
            return false;

        moveArea(overlay, -xOffset, -yOffset);
        dest = overlay;
        return true;
    }

    bool AABB2DOverlay(AABB2Di first, AABB2Di second, AABB2Di &dest)
    {
        using std::abs;
        using std::fmaxf;
        using std::fminf;

        if (!AABB2DInter(first, second))
            return false;

        Vector2i offset{ int32_t(fmaxf(abs(first.Min.x), abs(second.Min.x))),
                         int32_t(fmaxf(abs(first.Min.y), abs(second.Min.y))) };

        first.Move(offset);
        second.Move(offset);

        Rainy::AABB2Di overlay = { { int32_t(fmaxf(first.Min.x, second.Min.x)),
                                     int32_t(fmaxf(first.Min.y, second.Min.y)) },
                                   { int32_t(fminf(first.Max.x, second.Max.x)),
                                     int32_t(fminf(first.Max.y, second.Max.y)) } };

        // if (overlay.Max.x - overlay.Min.x < 0 || overlay.Max.y - overlay.Min.x < 0) return false;

        RN_ASSERT((overlay.Max.x - overlay.Min.x) >= 0, "(overlay.Max.x - overlay.Min.x) < 0");
        RN_ASSERT((overlay.Max.y - overlay.Min.y) >= 0, "(overlay.Max.y - overlay.Min.y) < 0");

        overlay.Move(-offset);
        dest = overlay;

        return true;
    }

    std::pair<AABB2Df, AABB2Df> AABB2DSplit(AABB2Df aabb, float splitVal, bool hSplit)
    {
        std::pair<AABB2Df, AABB2Df> result;

        /*if (aabb.max.x < point.x || aabb.max.y < point.y) return result;
        if (aabb.min.x > point.x || aabb.min.y > point.y) return result;
        if (aabb.max == point || aabb.min == point) return result;*/

        if (hSplit)
        {
            if (splitVal < aabb.Min.y && splitVal > aabb.Max.y)
                return result;

            result.first = AABB2Df{ aabb.Min, { aabb.Max.x, splitVal } };
            result.second = AABB2Df{ { aabb.Min.x, splitVal }, aabb.Max };
        }
        else
        {
            if (splitVal < aabb.Min.x && splitVal > aabb.Max.x)
                return result;

            result.first = AABB2Df{ aabb.Min, { splitVal, aabb.Max.y } };
            result.second = AABB2Df{ { splitVal, aabb.Min.y }, aabb.Max };
        }

        return result;
    }

    std::pair<AABB2Di, AABB2Di> AABB2DSplit(AABB2Di aabb, int32_t splitVal, bool hSplit)
    {
        std::pair<AABB2Di, AABB2Di> result;

        /*if (aabb.Max.x < point.x || aabb.Max.y < point.y) return result;
        if (aabb.Min.x > point.x || aabb.Min.y > point.y) return result;
        if (aabb.Max == point || aabb.Min == point) return result;*/

        if (hSplit)
        {
            if (splitVal < aabb.Min.y && splitVal > aabb.Max.y)
                return result;

            result.first = AABB2Di{ aabb.Min, { aabb.Max.x, splitVal } };
            result.second = AABB2Di{ { aabb.Min.x, splitVal }, aabb.Max };
        }
        else
        {
            if (splitVal < aabb.Min.x && splitVal > aabb.Max.x)
                return result;

            result.first = AABB2Di{ aabb.Min, { splitVal, aabb.Max.y } };
            result.second = AABB2Di{ { splitVal, aabb.Min.y }, aabb.Max };
        }

        return result;
    }

    std::vector<AABB2Di> AABB2DExcludeOverlay(AABB2Di aabb, AABB2Di overlay)
    {
        std::vector<AABB2Di> parts;

        auto split = AABB2DSplit(aabb, overlay.Min.y, true);
        if (!AABB2DZeroSpace(split.first))
            parts.push_back(split.first);

        split = AABB2DSplit(split.second, overlay.Max.y, true);
        if (!AABB2DZeroSpace(split.second))
            parts.push_back(split.second);

        split = AABB2DSplit(split.first, overlay.Min.x, false);
        if (!AABB2DZeroSpace(split.first))
            parts.push_back(split.first);

        split = AABB2DSplit(split.second, overlay.Max.x, false);
        if (!AABB2DZeroSpace(split.second))
            parts.push_back(split.second);

        int32_t width = aabb.GetWidth();
        int32_t height = aabb.GetHeight();

        return parts;
    }
}
