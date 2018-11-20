/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Mathlib.inl -- Copyright (c) 2005-2006 David Henry
// last modification: feb. 1, 2006
//
// -------------------------------------------------------------------
// Portions Copyright (c) Dante Treglia II and Mark A. DeLoura, 2000.
// Portions Copyright (c) Fletcher Dunn and Ian Parberry, 2002.
// -------------------------------------------------------------------
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Implementation of a math library to use with OpenGL.
//
// Provide vector, matrix and quaternion operations.
//
/////////////////////////////////////////////////////////////////////////////

#include <cassert>

///////////////////////////////////////////////////////////////////////////
//
// class Quaternion<Real> implementation.
//
/////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
// Quaternion::identity
//
// Set to identity
// --------------------------------------------------------------------------

template <typename Real>
inline void
Quaternion<Real>::identity () {

  _w = 1.0; _x = _y = _z = 0.0;
}


// --------------------------------------------------------------------------
// Quaternion::normalize
//
// "Normalize" a quaternion.  Note that normally, quaternions
// are always normalized (within limits of numerical precision).
//
// This function is provided primarily to combat floating point "error
// creep", which can occur when many successive quaternion operations
// are applied.
// --------------------------------------------------------------------------

template <typename Real>
inline void
Quaternion<Real>::normalize () {

	// Compute magnitude of the quaternion
	Real mag = std::sqrt ((_w * _w) + (_x * _x) + (_y * _y) + (_z * _z));

	// Check for bogus length, to protect against divide by zero
	if (mag > 0.0)
	{
		// Normalize it
		Real oneOverMag = 1.0 / mag;

		_w *= oneOverMag;
		_x *= oneOverMag;
		_y *= oneOverMag;
		_z *= oneOverMag;
	}
}

// --------------------------------------------------------------------------
// Quaternion<Real>::computeW
//
// Compute the W component of a unit quaternion given its x,y,z components.
// --------------------------------------------------------------------------

template <typename Real>
inline void
Quaternion<Real>::computeW () {

	Real t = 1.0 - (_x * _x) - (_y * _y) - (_z * _z);

	if (t < 0.0)
		_w = 0.0;
	else
		_w = -std::sqrt (t);
}

// --------------------------------------------------------------------------
// Quaternion<Real>::rotate
//
// Rotate a point by quaternion.  v' = q.p.q*, where p = <0, v>.
// --------------------------------------------------------------------------

template <typename Real>
inline void
Quaternion<Real>::rotate (Vector3 &v) const {

	Quaternion<Real> qf = *this * v * ~(*this);
	v.x = qf._x;
	v.y = qf._y;
	v.z = qf._z;
}

// --------------------------------------------------------------------------
// Quaternion::fromEulerAngles
//
// Setup the quaternion to perform an object->inertial rotation, given the
// orientation in XYZ-Euler angles format.  x,y,z parameters must be in
// radians.
// --------------------------------------------------------------------------

template <typename Real>
inline void
Quaternion<Real>::fromEulerAngles (Real x, Real y, Real z) {

	// Compute sine and cosine of the half angles
	Real sr = std::sin (x * 0.5);
	Real cr = std::cos (x * 0.5);
	Real sp = std::sin (y * 0.5);
	Real cp = std::cos (y * 0.5);
	Real sy = std::sin (z * 0.5);
	Real cy = std::cos (z * 0.5);

	// Compute values
	_w =  (cy * cp * cr) + (sy * sp * sr);
	_x = -(sy * sp * cr) + (cy * cp * sr);
	_y =  (cy * sp * cr) + (sy * cp * sr);
	_z = -(cy * sp * sr) + (sy * cp * cr);
}

// --------------------------------------------------------------------------
// Quaternion::toEulerAngles
//
// Setup the Euler angles, given an object->inertial rotation quaternion.
// Returned x,y,z are in radians.
// --------------------------------------------------------------------------

template <typename Real>
inline void
Quaternion<Real>::toEulerAngles (Real &x, Real &y, Real &z) const {

	// Compute Y-axis angle
	y = std::asin (2.0 * ((_x * _z) + (_w * _y)));

	// Compute cos and one over cos for optimization
	Real cy = std::cos (y);
	Real oneOverCosY = 1.0 / cy;

	if (std::fabs (cy) > 0.001) {

		// No gimball lock
		x = std::atan2 (2.0 * ((_w * _x) - (_y * _z)) * oneOverCosY,
							(1.0 - 2.0 * (_x*_x + _y*_y)) * oneOverCosY);
		z = std::atan2 (2.0 * ((_w * _z) - (_x * _y)) * oneOverCosY,
							(1.0 - 2.0 * (_y*_y + _z*_z)) * oneOverCosY);
	}
	else {

		// Gimbal lock case
		x = 0.0;
		z = std::atan2 (2.0 * ((_x * _y) + (_w * _z)),
							1.0 - 2.0 * (_x*_x + _z*_z));
	}
}

// --------------------------------------------------------------------------
// Quaternion::getRotationAngle
//
// Return the rotation angle theta (in radians).
// --------------------------------------------------------------------------

template <typename Real>
inline Real
Quaternion<Real>::rotationAngle () const
{
  // Compute the half angle.  Remember that w = cos(theta / 2)
  Real thetaOver2 = safeAcos (_w);

  // Return the rotation angle
  return thetaOver2 * 2.0;
}


// --------------------------------------------------------------------------
// Quaternion::getRotationAxis
//
// Return the rotation axis.
// --------------------------------------------------------------------------

template <typename Real>
inline Vector3
Quaternion<Real>::rotationAxis () const
{
  // Compute sin^2(theta/2).  Remember that w = cos(theta/2),
  // and sin^2(x) + cos^2(x) = 1
  Real sinThetaOver2Sq = 1.0 - (_w * _w);

  // Protect against numerical imprecision
  if (sinThetaOver2Sq <= 0.0)
    {
      // Identity quaternion, or numerical imprecision.  Just
      // return any valid vector, since it doesn't matter

      return Vector3 (1.0f, 0.0f, 0.0f);
    }

  // Compute 1 / sin(theta/2)
  Real oneOverSinThetaOver2 = 1.0 / std::sqrt (sinThetaOver2Sq);

  // Return axis of rotation
  return Vector3 (
	_x * oneOverSinThetaOver2,
	_y * oneOverSinThetaOver2,
	_z * oneOverSinThetaOver2
   );
}


// --------------------------------------------------------------------------
// Quaternion operators
//
// Operator overloading for basic quaternion operations.
// --------------------------------------------------------------------------

template <typename Real>
inline Quaternion<Real>
Quaternion<Real>::operator+ (const Quaternion<Real> &q) const
{
  return Quaternion<Real> (_w + q._w, _x + q._x, _y + q._y, _z + q._z);
}

template <typename Real>
inline Quaternion<Real> &
Quaternion<Real>::operator+= (const Quaternion<Real> &q)
{
  _w += q._w; _x += q._x; _y += q._y; _z += q._z;
  return *this;
}

template <typename Real>
inline Quaternion<Real>
Quaternion<Real>::operator- (const Quaternion<Real> &q) const
{
  return Quaternion<Real> (_w - q._w, _x - q._x, _y - q._y, _z - q._z);
}

template <typename Real>
inline Quaternion<Real> &
Quaternion<Real>::operator-= (const Quaternion<Real> &q)
{
  _w -= q._w; _x -= q._x; _y -= q._y; _z -= q._z;
  return *this;
}

// Quaternion multiplication.  The order of multiplication, from left
// to right, corresponds to the order of concatenated rotations.
// NOTE: Quaternion multiplication is NOT commutative, p * q != q * p
template <typename Real>
inline Quaternion<Real>
Quaternion<Real>::operator* (const Quaternion<Real> &q) const
{
  // We use the Grassman product formula:
  // pq = <w1w2 - dot(v1, v2), w1v2 + w2v1 + cross(v1, v2)>
  return Quaternion<Real> (
	(_w * q._w) - (_x * q._x) - (_y * q._y) - (_z * q._z),
	(_x * q._w) + (_w * q._x) + (_y * q._z) - (_z * q._y),
	(_y * q._w) + (_w * q._y) + (_z * q._x) - (_x * q._z),
	(_z * q._w) + (_w * q._z) + (_x * q._y) - (_y * q._x)
   );
}

template <typename Real>
inline Quaternion<Real> &
Quaternion<Real>::operator*= (const Quaternion<Real> &q)
{
  *this = *this * q;
  return *this;
}

template <typename Real>
inline Quaternion<Real>
Quaternion<Real>::operator* (const Vector3 &v) const
{
  // q * v = q * p where p = <0,v>
  // Thus, we can simplify the operations.
  return Quaternion<Real> (
	- (_x * v.x) - (_y * v.y) - (_z * v.z),
	  (_w * v.x) + (_y * v.z) - (_z * v.y),
	  (_w * v.y) + (_z * v.x) - (_x * v.z),
	  (_w * v.z) + (_x * v.y) - (_y * v.x)
   );
}

template <typename Real>
inline Quaternion<Real> &
	Quaternion<Real>::operator*= (const Vector3 &v)
{
	*this = *this * v;
	return *this;
}

template <typename Real>
inline Quaternion<Real>
Quaternion<Real>::operator* (Real k) const
{
  return Quaternion<Real> (_w * k, _x * k, _y * k, _z * k);
}

template <typename Real>
inline Quaternion<Real> &
Quaternion<Real>::operator*= (Real k)
{
  _w *= k; _x *= k; _y *= k; _z *= k;
  return *this;
}

template <typename Real>
inline Quaternion<Real>
Quaternion<Real>::operator/ (Real k) const
{
  Real oneOverK = 1.0 / k;
  return Quaternion<Real> (_w * oneOverK, _x * oneOverK, _y * oneOverK, _z * oneOverK);
}

template <typename Real>
inline Quaternion<Real> &
Quaternion<Real>::operator/= (Real k)
{
  Real oneOverK = 1.0 / k;
  _w *= oneOverK; _x *= oneOverK; _y *= oneOverK; _z *= oneOverK;
  return *this;
}

// Quaternion conjugate
template <typename Real>
inline Quaternion<Real>
Quaternion<Real>::operator~ () const
{
  return Quaternion<Real> (_w, -_x, -_y, -_z);
}


// Quaternion negation
template <typename Real>
inline Quaternion<Real>
Quaternion<Real>::operator- () const
{
  return Quaternion<Real> (-_w, -_x, -_y, -_z);
}


// --------------------------------------------------------------------------
//
// Nonmember Quaternion functions
//
// --------------------------------------------------------------------------

// Scalar on left multiplication
template <typename Real>
inline Quaternion<Real>
operator* (Real k, const Quaternion<Real> &q)
{
  return Quaternion<Real> (q._w * k, q._x * k, q._y * k, q._z * k);
}

// Quaternion dot product
template <typename Real>
inline Real
DotProduct (const Quaternion<Real> &a, const Quaternion<Real> &b)
{
  return ((a._w * b._w) + (a._x * b._x) + (a._y * b._y) + (a._z * b._z));
}

// Compute the quaternion conjugate.  This is the quaternian
// with the opposite rotation as the original quaternion.
template <typename Real>
inline Quaternion<Real>
Conjugate (const Quaternion<Real> &q)
{
  return Quaternion<Real> (q._w, -q._x, -q._y, -q._z);
}


// Compute the inverse quaternion (for unit quaternion only).
template <typename Real>
inline Quaternion<Real>
Inverse (const Quaternion<Real> &q)
{
  // Assume this is a unit quaternion! No check for this!
  Quaternion<Real> res (q._w, -q._x, -q._y, -q._z);
  res.normalize ();
  return res;
}


// --------------------------------------------------------------------------
// RotationQuaternion
//
// Setup the quaternion to rotate about the specified axis.  theta must
// be in radians.
// --------------------------------------------------------------------------

template <typename Real>
inline Quaternion<Real>
RotationQuaternion (Axis axis, Real theta)
{
  Quaternion<Real> res;

  // Compute the half angle
  Real thetaOver2 = theta * 0.5;

  // Set the values
  switch (axis)
    {
    case kXaxis:
      res._w = std::cos (thetaOver2);
      res._x = std::sin (thetaOver2);
      res._y = 0.0;
      res._z = 0.0;
      break;

    case kYaxis:
      res._w = std::cos (thetaOver2);
      res._x = 0.0;
      res._y = std::sin (thetaOver2);
      res._z = 0.0;
      break;

    case kZaxis:
      res._w = std::cos (thetaOver2);
      res._x = 0.0;
      res._y = 0.0;
      res._z = std::sin (thetaOver2);
      break;

    default:
      // Bad axis
      assert (false);
  }

  return res;
}

template <typename Real>
inline Quaternion<Real>
RotationQuaternion (const Vector3 &axis, Real theta)
{
  Quaternion<Real> res;

  // The axis of rotation must be normalized
  assert (std::fabs (DotProduct (axis, axis) - 1.0) < 0.001);

  // Compute the half angle and its sin
  Real thetaOver2 = theta * 0.5;
  Real sinThetaOver2 = std::sin (thetaOver2);

  // Set the values
  res._w = std::cos (thetaOver2);
  res._x = axis.x * sinThetaOver2;
  res._y = axis.y * sinThetaOver2;
  res._z = axis.z * sinThetaOver2;

  return res;
}


// --------------------------------------------------------------------------
// Log
//
// Unit quaternion logarithm. log(q) = log(cos(theta) + n*sin(theta))
// --------------------------------------------------------------------------

template <typename Real>
inline Quaternion<Real>
Log (const Quaternion<Real> &q)
{
  Quaternion<Real> res;
  res._w = 0.0;

  if (std::fabs (q._w) < 1.0)
    {
      Real theta = std::acos (q._w);
      Real sin_theta = std::sin (theta);

      if (std::fabs (sin_theta) > 0.00001)
	{
	  Real thetaOverSinTheta = theta / sin_theta;
	  res._x = q._x * thetaOverSinTheta;
	  res._y = q._y * thetaOverSinTheta;
	  res._z = q._z * thetaOverSinTheta;
	  return res;
	}
    }

  res._x = q._x;
  res._y = q._y;
  res._z = q._z;

  return res;
}


// --------------------------------------------------------------------------
// Exp
//
// Quaternion exponential.
// --------------------------------------------------------------------------

template <typename Real>
inline Quaternion<Real>
Exp (const Quaternion<Real> &q)
{
  Real theta = std::sqrt (DotProduct (q, q));
  Real sin_theta = std::sin (theta);

  Quaternion<Real> res;
  res._w = std::cos (theta);

  if (std::fabs (sin_theta) > 0.00001)
    {
      Real sinThetaOverTheta = sin_theta / theta;
      res._x = q._x * sinThetaOverTheta;
      res._y = q._y * sinThetaOverTheta;
      res._z = q._z * sinThetaOverTheta;
    }
  else
    {
      res._x = q._x;
      res._y = q._y;
      res._z = q._z;
    }

  return res;
}


// --------------------------------------------------------------------------
// Pow
//
// Quaternion exponentiation.
// --------------------------------------------------------------------------

template <typename Real>
inline Quaternion<Real>
Pow (const Quaternion<Real> &q, Real exponent)
{
  // Check for the case of an identity quaternion.
  // This will protect against divide by zero
  if (std::fabs (q._w) > 0.9999)
    return q;

  // Extract the half angle alpha (alpha = theta/2)
  Real alpha = std::acos (q._w);

  // Compute new alpha value
  Real newAlpha = alpha * exponent;

  // Compute new quaternion
  Vector3_<Real> n (q._x, q._y, q._z);
  n *= std::sin (newAlpha) / std::sin (alpha);

  return Quaternion<Real> (std::cos (newAlpha), n);
}


// --------------------------------------------------------------------------
// Slerp
//
// Spherical linear interpolation.
// --------------------------------------------------------------------------

template <typename Real>
inline Quaternion<Real>
Slerp (const Quaternion<Real> &q0, const Quaternion<Real> &q1, Real t)
{
	/*
								sin(1 - t) * THETA					sin ( t * THETA )
	slerp(q0, q1, t) =	------------------- * q0 +	------------------- * q1
								sin( THETA )							   sin( THETA )

		where, THETA = angle between q0 & q1
	*/
	///*
	// Check for out-of range parameter and return edge points if so
	if (t <= 0.0) return q0;
	if (t >= 1.0) return q1;

	// Compute "cosine of angle between quaternions" using dot product
	float cosTheta = DotProduct (q0, q1);

	float q1w = q1._w;
	float q1x = q1._x;
	float q1y = q1._y;
	float q1z = q1._z;

	// If negative dot (angle between q0 & q1 is obtuse), use -q1.  Two quaternions q and -q
	// represent the same rotation, but may produce different slerp.
	// We chose q or -q to rotate using the acute angle.
	if( cosTheta  < 0.0f ) { // THETA between q0 & q1 is OBTUSE
		q1w = -q1w;
		q1x = -q1x;
		q1y = -q1y;
		q1z = -q1z;
		cosTheta = -cosTheta;
	}

	// We should have two unit quaternions, so dot should be <= 1.0
	assert ( cosTheta < 1.1);
	
	// Compute interpolation fraction, checking for quaternions
	// almost exactly the same
	float k0, k1;

	if( cosTheta > 0.9999 ) { // The 2 quaternions are very close to each other, hence use linear interpolation
		k0 = 1.0f - t;
		k1 = t;
	}
	else {
		// Compute the sin of the angle using the
		// trig identity sin^2(omega) + cos^2(omega) = 1
		float sinTheta = std::sqrtf( 1 - (cosTheta * cosTheta) );

		// Compute the angle from its sin and cosine
		float theta = std::atan2f( sinTheta, cosTheta );

		// Compute inverse of denominator, so we only have to divide once
		float oneOverSinTheta = 1.0f / sinTheta;

		// Compute interpolation parameters
		k0 = std::sin( ( 1.0f - t ) * theta ) * oneOverSinTheta;
		k1 = std::sin( t * theta ) * oneOverSinTheta;
	}

	// Interpolate and return new quaternion
	Quaternion<float> q2(	( k0 * q0._w) + ( k1 * q1w),
									( k0 * q0._x) +  ( k1 * q1x),
									( k0 * q0._y) + ( k1 * q1y),
									( k0 * q0._z) + ( k1 * q1z) );

	return q2;
}


// --------------------------------------------------------------------------
// Squad
//
// Spherical cubic interpolation.
// squad = slerp (slerp (q0, q1, t), slerp (qa, qb, t), 2t(1 - t)).
// --------------------------------------------------------------------------

template <typename Real>
inline Quaternion<Real>
Squad (const Quaternion<Real> &q0, const Quaternion<Real> &qa,
       const Quaternion<Real> &qb, const Quaternion<Real> &q1, Real t)
{
  Real slerp_t = 2.0 * t * (1.0 - t);

  Quaternion<Real> slerp_q0 = Slerp (q0, q1, t);
  Quaternion<Real> slerp_q1 = Slerp (qa, qb, t);

  return Slerp (slerp_q0, slerp_q1, slerp_t);
}


// --------------------------------------------------------------------------
// Intermediate
//
// Compute intermediate quaternions for building spline segments.
// --------------------------------------------------------------------------

template <typename Real>
inline void
Intermediate (const Quaternion<Real> &qprev, const Quaternion<Real> &qcurr,
	      const Quaternion<Real> &qnext, Quaternion<Real> &qa, Quaternion<Real> &qb)
{
  // We should have unit quaternions
  assert (DotProduct (qprev, qprev) <= 1.0001);
  assert (DotProduct (qcurr, qcurr) <= 1.0001);

  Quaternion<Real> inv_prev = Conjugate (qprev);
  Quaternion<Real> inv_curr = Conjugate (qcurr);

  Quaternion<Real> p0 = inv_prev * qcurr;
  Quaternion<Real> p1 = inv_curr * qnext;

  Quaternion<Real> arg = (Log (p0) - Log (p1)) * 0.25;

  qa = qcurr * Exp ( arg);
  qb = qcurr * Exp (-arg);
}
