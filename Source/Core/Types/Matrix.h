#pragma once

#include "BasicTypes.h"
#include "Vector3.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class TMatrix : public glm::mat4
{
public:
    static const TMatrix Identity;

public:
  	template <typename... Args>
	FORCEINLINE TMatrix(Args... args) : glm::mat4(args...) { }

	FORCEINLINE static TMatrix LookAt(TVec3 const& eyePosition, TVec3 const& lookAtPosition, TVec3 const& upVector)
	{
		return glm::lookAt(
            glm::vec3(eyePosition.X, eyePosition.Y, eyePosition.Z),
            glm::vec3(lookAtPosition.X, lookAtPosition.Y, lookAtPosition.Z),
            glm::vec3(upVector.X, upVector.Y, upVector.Z)
        );
	}

	FORCEINLINE static TMatrix Translate(TMatrix const& matrix, TVec3 const& translation)
	{
		return glm::translate(
            matrix,
            glm::vec3(translation.X, translation.Y, translation.Z)
        );
	}

	FORCEINLINE static TMatrix Rotate(TMatrix const& matrix, float const angle, TVec3 const& axis)
	{
		return glm::rotate(
            matrix,
            angle,
            glm::vec3(axis.X, axis.Y, axis.Z)
        );
	}

	FORCEINLINE static TMatrix Scale(TMatrix const& matrix, TVec3 const& scale)
	{
		return glm::scale(
            matrix,
            glm::vec3(scale.X, scale.Y, scale.Z)
        );
	}

	FORCEINLINE static TMatrix Perspective(float fovY, float aspect, float zNear, float zFar)
	{
		return glm::perspective(fovY, aspect, zNear, zFar);
	}

	FORCEINLINE static TMatrix Orthographic(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		return glm::ortho(left, right, bottom, top, zNear, zFar);
	}

	static TMatrix Lerp(TMatrix const& from, TMatrix const& to, float alpha)
	{
		float ret[16] = { 0.0 };
		const float* f = from.GetPtr();
		const float* t = to.GetPtr();
		for (uint8 i = 0; i < 16; i++)
			ret[i] = f[i] + alpha * (t[i] - f[i]);

		return glm::make_mat4(ret);
	}

    void SetPosition(TVec3 const& position)
    {
        float* valuePtr = glm::value_ptr(*(static_cast<glm::mat4*>(this)));
        valuePtr[12] = position.X;
        valuePtr[13] = position.Y;
        valuePtr[14] = position.Z;
    }

	FORCEINLINE float const* GetPtr() const
	{
		return glm::value_ptr(*(static_cast<const glm::mat4*>(this)));
	}
};
