#include "B2D_pch.h"
#include "Transform.h"

Transform::Transform()
    : m_position(0.0f, 0.0f, 0.0f)
    , m_scale(1.0f)
{

}

void Transform::SetPosition(TVec3 const& position)
{
    m_position = position;
}

void Transform::SetScale(TVec3 const& scale)
{
    m_scale = scale;
}

void Transform::CalculateMatrix(TMatrix& matrix)
{
    matrix = TMatrix(1.0f);
    matrix = TMatrix::Translate(matrix, m_position);
    matrix = TMatrix::Scale(matrix, m_scale);
}
