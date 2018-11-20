/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Mathlib.h -- Copyright (c) 2005-2006 David Henry
// last modification: feb. 19, 2006
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Declarations for 3D maths object and functions to use with OpenGL.
//
// Provide vector, matrix and quaternion operations.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef QUATERNION_H
#define QUATERNION_H

#include "Common/Vectors.h"
#include <cmath>

// Forward declarations
template <typename Real> class Quaternion;

// Type definitions
enum Axis {
  kXaxis, kYaxis, kZaxis
};

/////////////////////////////////////////////////////////////////////////////
//
// class Quaternion<Real> - Implement a quaternion, for purposes of
// representing an angular displacement (orientation) in 3D.
//
/////////////////////////////////////////////////////////////////////////////

template <typename Real>
class Quaternion {
	public:
		// Constructors
		Quaternion () { }
		Quaternion (Real w, Real x, Real y, Real z)
			: _w (w), _x (x), _y (y), _z (z) { }

	public:
		// Public interface
		void identity ();
		void normalize ();
		void computeW ();
		void rotate (Vector3 &v) const;

		// Quaternion <-> Euler conversions; XYZ rotation order; angles in radians
		void fromEulerAngles (Real x, Real y, Real z);
		void toEulerAngles (Real &x, Real &y, Real &z) const;

		Real rotationAngle () const;
		Vector3 rotationAxis () const;

		// Quaternion operations
		Quaternion<Real> operator+ (const Quaternion<Real> &q) const;
		Quaternion<Real> &operator+= (const Quaternion<Real> &q);

		Quaternion<Real> operator- (const Quaternion<Real> &q) const;
		Quaternion<Real> &operator-= (const Quaternion<Real> &q);

		Quaternion<Real> operator* (const Quaternion<Real> &q) const;
		Quaternion<Real> &operator*= (const Quaternion<Real> &q);

		Quaternion<Real> operator* (Real k) const;
		Quaternion<Real> &operator*= (Real k);

		Quaternion<Real> operator* (const Vector3 &v) const;
		Quaternion<Real> &operator*= (const Vector3 &v);

		Quaternion<Real> operator/ (Real k) const;
		Quaternion<Real> &operator/= (Real k);

		Quaternion<Real> operator~ () const; // Quaternion conjugate
		Quaternion<Real> operator- () const; // Quaternion negation

		public:
			// Member variables

			// The 4 values of the quaternion.  Normally, it will not
			// be necessary to manipulate these directly.  However,
			// we leave them public, since prohibiting direct access
			// makes some operations, such as file I/O, unnecessarily
			// complicated.

		union {
			struct {
				Real _w, _x, _y, _z;
			};

			Real _q[4];
		};
};

// Predefined Quaternion types
typedef Quaternion<float> Quaternionf;
typedef Quaternion<double> Quaterniond;

// A global "identity" quaternion constant
static const Quaternionf kQuaternionIdentityf (1.0f, 0.0f, 0.0f, 0.0f);
static const Quaterniond kQuaternionIdentityd (1.0f, 0.0f, 0.0f, 0.0f);

//
// Nonmember Matrix4x functions
//

template <typename Real>
Quaternion<Real> operator* (Real k, const Quaternion<Real> &q);

template <typename Real>
Real DotProduct (const Quaternion<Real> &a, const Quaternion<Real> &b);

template <typename Real>
Quaternion<Real> Conjugate (const Quaternion<Real> &q);

template <typename Real>
Quaternion<Real> Inverse (const Quaternion<Real> &q);

template <typename Real>
Quaternion<Real> RotationQuaternion (Axis axis, Real theta);

template <typename Real>
Quaternion<Real> RotationQuaternion (const Vector3 &axis, Real theta);

template <typename Real>
Quaternion<Real> Log (const Quaternion<Real> &q);
template <typename Real>
Quaternion<Real> Exp (const Quaternion<Real> &q);
template <typename Real>
Quaternion<Real> Pow (const Quaternion<Real> &q, Real exponent);

template <typename Real>
Quaternion<Real> Slerp (const Quaternion<Real> &q0, const Quaternion<Real> &q1, Real t);
template <typename Real>
Quaternion<Real> Squad (const Quaternion<Real> &q0, const Quaternion<Real> &qa,
									const Quaternion<Real> &qb, const Quaternion<Real> &q1, Real t);
template <typename Real>
inline void Intermediate (	const Quaternion<Real> &qprev, const Quaternion<Real> &qcurr,
									const Quaternion<Real> &qnext, Quaternion<Real> &qa,
									Quaternion<Real> &qb);

// Include inline function definitions
#include "Quaternion.inl"

#endif // QUATERNION_H
