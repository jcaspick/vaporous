#pragma once
#include "Mesh.h"
#include "RoadSegment.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace MeshUtil {
	inline Mesh arcCW(float radius, float angle, float width, int divisions,
		float distanceOffset, float arcLength, HeightMap* heightmap) {
		Mesh arc;
		float angleStep = angle / divisions;
		float distanceStep = arcLength / divisions;

		for (int d = 0; d < divisions + 1; ++d) {
			float angle = glm::radians(d * angleStep);
			float distance = distanceOffset + d * distanceStep;

			Vertex left;
			left.position = vec3(cos(angle), 0, sin(angle)) 
				* (radius + width * 0.5f) - vec3(radius, 0, 0);
			left.uv = vec2(0, distance);

			Vertex right;
			right.position = vec3(cos(angle), 0, sin(angle))
				* (radius - width * 0.5f) - vec3(radius, 0, 0);
			right.uv = vec2(1, distance);

			left.position.y = heightmap->sample(distance);
			right.position.y = heightmap->sample(distance);

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

	inline Mesh arcCCW(float radius, float angle, float width, int divisions,
		float distanceOffset, float arcLength, HeightMap* heightmap) {
		Mesh arc;
		float angleStep = -angle / divisions;
		float distanceStep = arcLength / divisions;

		for (int d = 0; d < divisions + 1; ++d) {
			float angle = glm::radians(d * angleStep - 180);
			float distance = distanceOffset + d * distanceStep;

			Vertex left;
			left.position = vec3(cos(angle), 0, sin(angle))
				* (radius - width * 0.5f) + vec3(radius, 0, 0);
			left.uv = vec2(0, distance);

			Vertex right;
			right.position = vec3(cos(angle), 0, sin(angle))
				* (radius + width * 0.5f) + vec3(radius, 0, 0);
			right.uv = vec2(1, distance);

			left.position.y = heightmap->sample(distance);
			right.position.y = heightmap->sample(distance);

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

	inline Mesh segmentMesh(RoadSegment segment, float width, float quality,
		HeightMap* heightmap) 
	{
		int divisions = ceil(quality * segment.arcLength());
		if (segment.orientation == Orientation::Left)
			return arcCCW(segment.radius, segment.angle, width, divisions,
				segment.distanceOffset(), segment.arcLength(), heightmap);
		else
			return arcCW(segment.radius, segment.angle, width, divisions,
				segment.distanceOffset(), segment.arcLength(), heightmap);
	}

	inline Mesh loadFromObj(const std::string& path, float scale = 1.0f) {
		using ivec3 = glm::ivec3;

		std::ifstream obj;
		obj.open(path);

		if (!obj.is_open()) {
			std::cout << "failed to load obj file at path " << path << std::endl;
			return Mesh();
		}

		std::vector<vec3> vertices;
		std::vector<vec3> normals;
		std::vector<vec2> uvs;
		std::vector<ivec3> faces;

		std::string line;
		while (std::getline(obj, line)) {
			std::stringstream sstream(line);
			std::string token;
			sstream >> token;

			// comment line
			if (token == "#") continue;

			// vertex position
			if (token == "v") {
				vec3 vertex;
				sstream >> vertex.x >> vertex.y >> vertex.z;
				vertex *= scale;
				vertices.push_back(vertex);
			}

			// vertex normal
			if (token == "vn") {
				vec3 normal;
				sstream >> normal.x >> normal.y >> normal.z;
				normals.push_back(normal);
			}

			// uv coord
			if (token == "vt") {
				vec2 uv;
				sstream >> uv.x >> uv.y;
				uvs.push_back(uv);
			}

			// face
			if (token == "f") {
				while (sstream) {
					std::string faceStr;
					sstream >> faceStr;
					if (faceStr == "") continue;

					std::stringstream faceStrStream(faceStr);
					std::string value;
					std::vector<std::string> values;
					while (std::getline(faceStrStream, value, '/')) {
						values.push_back(value);
					}

					ivec3 face;
					face.x = stoi(values.at(0)) - 1;
					face.y = stoi(values.at(1)) - 1;
					face.z = stoi(values.at(2)) - 1;

					faces.push_back(face);
				}
			}
		}

		// create unique vertices from obj indices
		Mesh mesh;
		for (auto face : faces) {
			mesh.vertices.emplace_back(
				Vertex(vertices.at(face.x), uvs.at(face.y),
					normals.at(face.z)));
		}
		for (int i = 0; i < mesh.vertices.size(); ++i) {
			mesh.indices.emplace_back(i);
		}
		return mesh;

		obj.close();
	}
}