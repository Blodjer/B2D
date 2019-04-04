#pragma once

#include "Core/Types/Matrix.h"
#include "Graphics/Material.h"

class RenderObject
{
public:
    RenderObject(TMatrix const& matrix, CMaterial const& material)
        : mMatrix(matrix)
        , mMaterial(material)
    {
    }

    TMatrix mMatrix;
    CMaterial mMaterial;

};