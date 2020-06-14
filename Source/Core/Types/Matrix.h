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
            glm::vec3(eyePosition.x, eyePosition.y, eyePosition.z),
            glm::vec3(lookAtPosition.x, lookAtPosition.y, lookAtPosition.z),
            glm::vec3(upVector.x, upVector.y, upVector.z)
        );
	}

	FORCEINLINE static TMatrix Translate(TMatrix const& matrix, TVec3 const& translation)
	{
		return glm::translate(
            matrix,
            glm::vec3(translation.x, translation.y, translation.z)
        );
	}

	FORCEINLINE static TMatrix Rotate(TMatrix const& matrix, float const angle, TVec3 const& axis)
	{
		return glm::rotate(
            matrix,
            angle,
            glm::vec3(axis.x, axis.y, axis.z)
        );
	}

	FORCEINLINE static TMatrix Scale(TMatrix const& matrix, TVec3 const& scale)
	{
		return glm::scale(
            matrix,
            glm::vec3(scale.x, scale.y, scale.z)
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
        valuePtr[12] = position.x;
        valuePtr[13] = position.y;
        valuePtr[14] = position.z;
    }

	FORCEINLINE float const* GetPtr() const
	{
		return glm::value_ptr(*(static_cast<const glm::mat4*>(this)));
	}
};
