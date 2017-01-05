//
// Copyright (c) 2016 CNRS
//
// This file is part of Pinocchio
// Pinocchio is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// Pinocchio is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// Pinocchio If not, see
// <http://www.gnu.org/licenses/>.

#ifndef __se3_special_orthogonal_operation_hpp__
#define __se3_special_orthogonal_operation_hpp__

#include <limits>

#include <pinocchio/spatial/explog.hpp>
#include <pinocchio/math/quaternion.hpp>
#include <pinocchio/multibody/liegroup/operation-base.hpp>

namespace se3
{
  struct SpecialOrthogonalOperation;
  template <> struct traits<SpecialOrthogonalOperation> {
    typedef double Scalar;
    enum {
      NQ = 4,
      NV = 3
    };
    typedef Eigen::Matrix<Scalar,NQ,1> ConfigVector_t;
    typedef Eigen::Matrix<Scalar,NV,1> TangentVector_t;
  };

  struct SpecialOrthogonalOperation : public LieGroupOperationBase <SpecialOrthogonalOperation>
  {
    typedef SpecialOrthogonalOperation LieGroupDerived;
    SE3_LIE_GROUP_TYPEDEF;

    typedef Eigen::Quaternion<Scalar> Quaternion_t;
    typedef Eigen::Map<      Quaternion_t> QuaternionMap_t;
    typedef Eigen::Map<const Quaternion_t> ConstQuaternionMap_t;

    template <class ConfigL_t, class ConfigR_t, class Tangent_t>
    static void difference_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                const Eigen::MatrixBase<ConfigR_t> & q1,
                                const Eigen::MatrixBase<Tangent_t> & d)
    {
      ConstQuaternionMap_t p0 (q0.derived().data());
      ConstQuaternionMap_t p1 (q1.derived().data());
      const_cast < Eigen::MatrixBase<Tangent_t>& > (d)
        = log3((p0.matrix().transpose() * p1.matrix()).eval());
    }

    template <class ConfigIn_t, class Velocity_t, class ConfigOut_t>
    static void integrate_impl(const Eigen::MatrixBase<ConfigIn_t> & q,
                               const Eigen::MatrixBase<Velocity_t> & v,
                               const Eigen::MatrixBase<ConfigOut_t> & qout)
    {
      ConstQuaternionMap_t quat(q.derived().data());
      QuaternionMap_t quaternion_result (
          (const_cast< Eigen::MatrixBase<ConfigOut_t>& >(qout)).derived().data()
          );

      Quaternion_t pOmega(exp3(v));
      quaternion_result = quat * pOmega;
      firstOrderNormalize(quaternion_result);
    }

    template <class ConfigL_t, class ConfigR_t, class ConfigOut_t>
    static void interpolate_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                 const Eigen::MatrixBase<ConfigR_t> & q1,
                                 const Scalar& u,
                                 const Eigen::MatrixBase<ConfigOut_t>& qout)
    {
      ConstQuaternionMap_t p0 (q0.derived().data());
      ConstQuaternionMap_t p1 (q1.derived().data());
      QuaternionMap_t quaternion_result (
          (const_cast< Eigen::MatrixBase<ConfigOut_t>& >(qout)).derived().data()
          );

      quaternion_result = p0.slerp(u, p1);
    }

    template <class ConfigL_t, class ConfigR_t>
    static double squaredDistance_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                       const Eigen::MatrixBase<ConfigR_t> & q1)
    {
      TangentVector_t t;
      difference_impl(q0, q1, t);
      return t.squaredNorm();
    }

    template <class Config_t>
    static void random_impl (const Eigen::MatrixBase<Config_t>& qout)
    {
      QuaternionMap_t out (
          (const_cast< Eigen::MatrixBase<Config_t>& >(qout)).derived().data()
          );
      uniformRandom(out);
    }

    template <class ConfigL_t, class ConfigR_t, class ConfigOut_t>
    static void randomConfiguration_impl(const Eigen::MatrixBase<ConfigL_t> &,
                                         const Eigen::MatrixBase<ConfigR_t> &,
                                         const Eigen::MatrixBase<ConfigOut_t> & qout)
    {
      random_impl(qout);
    }

    template <class ConfigL_t, class ConfigR_t>
    static bool isSameConfiguration_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                         const Eigen::MatrixBase<ConfigR_t> & q1,
                                         const Scalar & prec)
    {
      ConstQuaternionMap_t quat1(q0.derived().data());
      ConstQuaternionMap_t quat2(q1.derived().data());

      return defineSameRotation(quat1,quat2,prec);
    }
  }; // struct SpecialOrthogonalOperation

  struct SpecialOrthogonal1Operation;
  template <> struct traits<SpecialOrthogonal1Operation> {
    typedef double Scalar;
    enum {
      NQ = 2,
      NV = 1
    };
    typedef Eigen::Matrix<Scalar,NQ,1> ConfigVector_t;
    typedef Eigen::Matrix<Scalar,NV,1> TangentVector_t;
  };

  struct SpecialOrthogonal1Operation : public LieGroupOperationBase <SpecialOrthogonal1Operation>
  {
    typedef SpecialOrthogonal1Operation LieGroupDerived;
    SE3_LIE_GROUP_TYPEDEF;

    template <class ConfigL_t, class ConfigR_t, class Tangent_t>
    static void difference_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                const Eigen::MatrixBase<ConfigR_t> & q1,
                                const Eigen::MatrixBase<Tangent_t> & d)
    {
      const_cast < Eigen::MatrixBase<Tangent_t>& > (d) [0]
        = atan2 (q0(0)*q1(1) - q0(1)*q1(0), q0.dot(q1));
    }

    template <class ConfigIn_t, class Velocity_t, class ConfigOut_t>
    static void integrate_impl(const Eigen::MatrixBase<ConfigIn_t> & q,
                               const Eigen::MatrixBase<Velocity_t> & v,
                               const Eigen::MatrixBase<ConfigOut_t> & qout)
    {
      ConfigOut_t& out = (const_cast< Eigen::MatrixBase<ConfigOut_t>& >(qout)).derived();

      const Scalar & ca = q(0);
      const Scalar & sa = q(1);
      const Scalar & omega = v(0);

      Scalar cosOmega,sinOmega; SINCOS(omega, &sinOmega, &cosOmega);
      // TODO check the cost of atan2 vs SINCOS

      out << cosOmega * ca - sinOmega * sa,
             sinOmega * ca + cosOmega * sa;
      const Scalar norm2 = q.squaredNorm();
      out *= (3 - norm2) / 2;
    }

    template <class ConfigL_t, class ConfigR_t, class ConfigOut_t>
    static void interpolate_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                 const Eigen::MatrixBase<ConfigR_t> & q1,
                                 const Scalar& u,
                                 const Eigen::MatrixBase<ConfigOut_t>& qout)
    {
      ConfigOut_t& out = (const_cast< Eigen::MatrixBase<ConfigOut_t>& >(qout)).derived();

      assert ( (q0.norm() - 1) < 1e-8 && "initial configuration not normalized");
      assert ( (q1.norm() - 1) < 1e-8 && "final configuration not normalized");
      Scalar cosTheta = q0.dot(q1);
      Scalar sinTheta = q0(0)*q1(1) - q0(1)*q1(0);
      Scalar theta = atan2(sinTheta, cosTheta);
      assert (fabs (sin (theta) - sinTheta) < 1e-8);

      if (fabs (theta) > 1e-6 && fabs (theta) < PI - 1e-6)
      {
        out = (sin ((1-u)*theta)/sinTheta) * q0
            + (sin (   u *theta)/sinTheta) * q1;
      }
      else if (fabs (theta) < 1e-6) // theta = 0
      {
        out = (1-u) * q0 + u * q1;
      }
      else // theta = +-PI
      {
        double theta0 = atan2 (q0(1), q0(0));
        out << cos (theta0 + u * theta),
               sin (theta0 + u * theta);
      }
    }

    // template <class ConfigL_t, class ConfigR_t>
    // static double squaredDistance_impl(const Eigen::MatrixBase<ConfigL_t> & q0,
                                       // const Eigen::MatrixBase<ConfigR_t> & q1)

    template <class Config_t>
    static void random_impl (const Eigen::MatrixBase<Config_t>& qout)
    {
      Config_t& out = (const_cast< Eigen::MatrixBase<Config_t>& >(qout)).derived();
      const Scalar angle = -PI + 2*PI * ((Scalar)rand())/RAND_MAX;
      SINCOS (angle, &out(1), &out(0));
    }

    template <class ConfigL_t, class ConfigR_t, class ConfigOut_t>
    static void randomConfiguration_impl(const Eigen::MatrixBase<ConfigL_t> &,
                                         const Eigen::MatrixBase<ConfigR_t> &,
                                         const Eigen::MatrixBase<ConfigOut_t> & qout)
    {
      random_impl(qout);
    }
  }; // struct SpecialOrthogonal1Operation
} // namespace se3

#endif // ifndef __se3_special_orthogonal_operation_hpp__
