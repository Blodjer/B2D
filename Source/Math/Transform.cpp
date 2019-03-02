#include "B2D_pch.h"
#include "Transform.h"

Transform::Transform()
    : mPosition(0.0f, 0.0f, 0.0f)
    , mScale(1.0f)
{

}

void Transform::SetPosition(TVec3 const& position)
{
    mPosition = position;
}

void Transform::SetScale(TVec3 const& scale)
{
    mScale = scale;
}

void Transform::CalculateMatrix(TMatrix& matrix)
{
    matrix = TMatrix(1.0f);
    matrix = TMatrix::Translate(matrix, mPosition);
    matrix = TMatrix::Scale(matrix, mScale);
}
