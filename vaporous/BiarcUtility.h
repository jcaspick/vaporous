#pragma once
#include <glm/glm.hpp>

using vec3 = glm::vec3;

struct Biarc {
	Biarc(float r1, float a1, bool f1, float r2, float a2, float f2) :
		arc1_radius(r1),
		arc1_angle(a1),
		arc1_flipped(f1),
		arc2_radius(r2),
		arc2_angle(a2),
		arc2_flipped(f2)
	{}

	float arc1_radius;
	float arc1_angle;
	bool arc1_flipped;
	float arc2_radius;
	float arc2_angle;
	bool arc2_flipped;
};

namespace BiarcUtil {
	// helper function used by the biarc solver
	inline float xOp(vec3 a, vec3 b) {
		return a.x * b.z - a.z * b.x;
	}

	/// <summary>
	/// Computes a biarc between two points
	/// </summary>
	/// <param name = "p1">point 1</param>
	/// <param name = "t1">tangent 1, a normalized direction vector</param>
	/// <param name = "p2">point 2</param>
	/// <param name = "t2">tangent 2, a normalized direction vector</param>
	inline Biarc biarc(vec3 p1, vec3 t1, vec3 p2, vec3 t2) {
		// adapted from
		// http://www.ryanjuckett.com/programming/biarc-interpolation/

		// sanitize input
		// make sure we're working on the XZ plane with no height values
		// make sure tangents are normalized
		p1.y = 0;
		p2.y = 0;
		t1.y = 0;
		t2.y = 0;
		t1 = glm::normalize(t1);
		t2 = glm::normalize(t2);

		// useful values
		vec3 v = p2 - p1;
		vec3 t = t1 + t2;

		// d2, there is no d1 because both are assumed equal
		float d2 = 0;
		if ((2 * (1 - glm::dot(t1, t2))) == 0)
			d2 = glm::length2(v) / glm::dot(4.0f * v, t2);
		else
			d2 = (-glm::dot(v, t) + sqrt(pow(glm::dot(v, t), 2) + 2
				* (1 - glm::dot(t1, t2)) * glm::length2(v)))
			/ (2 * (1 - glm::dot(t1, t2)));

		// pm - the point where the two arcs of the biarc meet
		vec3 pm = (p1 + p2 + d2 * (t1 - t2)) / 2.0f;

		// n1 & n2 - perpendicular to t1 & t2
		vec3 n1 = glm::normalize(vec3(-t1.z, 0.0f, t1.x));
		vec3 n2 = glm::normalize(vec3(-t2.z, 0.0f, t2.x));

		// s1 & s2 - the absolute value of which are the radii of the arcs
		float s1 = glm::length2(pm - p1) / glm::dot(n1 * 2.0f, pm - p1);
		float s2 = glm::length2(pm - p2) / glm::dot(n2 * 2.0f, pm - p2);
		float r1 = abs(s1);
		float r2 = abs(s2);

		// c1 & c2 - the center of each arc
		vec3 c1 = p1 + s1 * n1;
		vec3 c2 = p2 + s2 * n2;

		// o1 & o2 - the angle of each arc
		float o1 = 0;
		float o2 = 0;
		vec3 op1 = (p1 - c1) / r1;
		vec3 om1 = (pm - c1) / r1;
		vec3 op2 = (p2 - c2) / r2;
		vec3 om2 = (pm - c2) / r2;

		// o1
		if (d2 > 0 && xOp(op1, om1) > 0)
			o1 = glm::acos(glm::dot(op1, om1));
		else if (d2 > 0 && xOp(op1, om1) <= 0)
			o1 = -glm::acos(glm::dot(op1, om1));
		else if (d2 <= 0 && xOp(op1, om1) > 0)
			o1 = -glm::two_pi<float>() + glm::acos(glm::dot(op1, om1));
		else if (d2 <= 0 && xOp(op1, om1) <= 0)
			o1 = glm::two_pi<float>() - glm::acos(glm::dot(op1, om1));

		// o2
		if (d2 > 0 && xOp(op2, om2) > 0)
			o2 = glm::acos(glm::dot(op2, om2));
		else if (d2 > 0 && xOp(op2, om2) <= 0)
			o2 = -glm::acos(glm::dot(op2, om2));
		else if (d2 <= 0 && xOp(op2, om2) > 0)
			o2 = -glm::two_pi<float>() + glm::acos(glm::dot(op2, om2));
		else if (d2 <= 0 && xOp(op2, om2) <= 0)
			o2 = glm::two_pi<float>() - glm::acos(glm::dot(op2, om2));

		// pack results into biarc struct
		float a1 = abs(glm::degrees(o1));
		bool f1 = o1 < 0 ? true : false;
		float a2 = abs(glm::degrees(o2));
		bool f2 = o2 >= 0 ? true : false;

		return Biarc(r1, a1, f1, r2, a2, f2);
	}
}