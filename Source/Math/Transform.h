#pragma once

class Transform
{
public:
    Transform();

    void SetPosition(TVec3 const& position);
    void SetScale(TVec3 const& scale);

    TVec3 const& GetPosition() const { return mPosition; }
    TVec3 const& GetScale() const { return mScale; }

    void CalculateMatrix(TMatrix& matrix);

private:
    TVec3 mPosition;
    TVec3 mScale;
};