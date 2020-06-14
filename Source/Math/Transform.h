#pragma once

class Transform
{
public:
    Transform();

    void SetPosition(TVec3 const& position);
    void SetScale(TVec3 const& scale);

    TVec3 const& GetPosition() const { return m_position; }
    TVec3 const& GetScale() const { return m_scale; }

    void CalculateMatrix(TMatrix& matrix);

private:
    TVec3 m_position;
    TVec3 m_scale;
};