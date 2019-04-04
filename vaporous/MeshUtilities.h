#pragma once
#include "MeshData.h"
#include <vector>

namespace MeshUtil {
	inline Mesh arcCW(float radius, float angle, float width, int divisions) {
		Mesh arc;
		float angleStep = angle / divisions;

		for (int d = 0; d < divisions + 1; ++d) {
			float angle = glm::radians(d * angleStep);

			Vertex left;
			left.position = vec3(cos(angle), 0, sin(angle)) 
				* (radius + width * 0.5f) - vec3(radius, 0, 0);
			left.uv = vec2(0, static_cast<float>(d) / divisions);

			Vertex right;
			right.position = vec3(cos(angle), 0, sin(angle))
				* (radius - width * 0.5f) - vec3(radius, 0, 0);
			right.uv = vec2(1, static_cast<float>(d) / divisions);

			arc.vertices.emplace_back(left);
			arc.vertices.emplace_back(right);
		}

		for (int i = 0; i < divisions * 2; i += 2) {
			arc.indices.emplace_back(i);
			arc.indices.emplace_back(i + 1);
			arc.indices.emplace_back(i + 2);
			arc.indices.emplace_back(i + 2);
			arc.indices.emplace_back(i + 1);
			arc.indices.emplace_back(i + 3);
		}

		return arc;
	}

	inline Mesh arcCCW(float radius, float angle, float width, int divisions) {
		Mesh arc;
		float angleStep = -angle / divisions;

		for (int d = 0; d < divisions + 1; ++d) {
			float angle = glm::radians(d * angleStep - 180);

			Vertex left;
			left.position = vec3(cos(angle), 0, sin(angle))
				* (radius - width * 0.5f) + vec3(radius, 0, 0);
			left.uv = vec2(0, static_cast<float>(d) / divisions);

			Vertex right;
			right.position = vec3(cos(angle), 0, sin(angle))
				* (radius + width * 0.5f) + vec3(radius, 0, 0);
			right.uv = vec2(1, static_cast<float>(d) / divisions);

			arc.vertices.emplace_back(left);
			arc.vertices.emplace_back(right);
		}

		for (int i = 0; i < divisions * 2; i += 2) {
			arc.indices.emplace_back(i);
			arc.indices.emplace_back(i + 1);
			arc.indices.emplace_back(i + 2);
			arc.indices.emplace_back(i + 2);
			arc.indices.emplace_back(i + 1);
			arc.indices.emplace_back(i + 3);
		}

		return arc;
	}
}