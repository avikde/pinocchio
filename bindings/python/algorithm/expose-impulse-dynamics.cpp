//
// Copyright (c) 2020-2021 CNRS INRIA
//

#include "pinocchio/bindings/python/algorithm/algorithms.hpp"
#include "pinocchio/bindings/python/utils/std-vector.hpp"
#include "pinocchio/algorithm/impulse-dynamics.hpp"

namespace bp = boost::python;

namespace pinocchio
{
    namespace python
    {
    
      typedef PINOCCHIO_STD_VECTOR_WITH_EIGEN_ALLOCATOR(context::RigidContactModel) RigidContactModelVector;
      typedef PINOCCHIO_STD_VECTOR_WITH_EIGEN_ALLOCATOR(context::RigidContactData) RigidContactDataVector;
    
      static const context::VectorXs impulseDynamics_proxy(const context::Model & model,
                                                           context::Data & data,
                                                           const context::VectorXs & q,
                                                           const context::VectorXs & v,
                                                           const RigidContactModelVector & contact_models,
                                                           RigidContactDataVector & contact_datas,
                                                           const context::Scalar & r_coeff =0.0,
                                                           const context::Scalar & mu = 0.0)
      {
        return impulseDynamics(model, data, q, v, contact_models, contact_datas, r_coeff, mu);
      }
    
      void exposeImpulseDynamics()
      {
        bp::def("impulseDynamics",impulseDynamics_proxy,
                (bp::arg("model"),bp::arg("data"),bp::arg("q"),bp::arg("v"),bp::arg("contact_models"),bp::arg("contact_datas"),bp::arg("r_coeff") = 0,bp::arg("mu") = 0),
                 "Computes the impulse dynamics with contact constraints according to a given list of Contact information.\n"
                 "When using impulseDynamics for the first time, you should call first initContactDynamics to initialize the internal memory used in the algorithm.\n"
                 "This function returns the after-impulse velocity of the system. The impulses acting on the contacts are stored in the list data.contact_forces.");
      }
    }
}

