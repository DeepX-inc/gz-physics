/*
 * Copyright (C) 2019 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef IGNITION_PHYSICS_BULLET_SRC_SDFFEATURES_HH_
#define IGNITION_PHYSICS_BULLET_SRC_SDFFEATURES_HH_

#include <string>

#include <ignition/physics/sdf/ConstructWorld.hh>
#include <ignition/physics/sdf/ConstructModel.hh>
#include <ignition/physics/sdf/ConstructLink.hh>
#include <ignition/physics/sdf/ConstructJoint.hh>
#include <ignition/physics/sdf/ConstructCollision.hh>
#include <ignition/physics/sdf/ConstructVisual.hh>
#include <ignition/physics/sdf/FinalizeModels.hh>

#include <ignition/physics/Implements.hh>

#include "EntityManagementFeatures.hh"

namespace ignition {
namespace physics {
namespace bullet {

using SDFFeatureList = FeatureList<
  sdf::ConstructSdfWorld,
  sdf::ConstructSdfModel,
  sdf::ConstructSdfLink,
  sdf::ConstructSdfJoint,
  sdf::ConstructSdfCollision,
  sdf::ConstructSdfVisual,
  sdf::FinalizeSdfModels
>;

class SDFFeatures :
    public virtual EntityManagementFeatures,
    public virtual Implements3d<SDFFeatureList>
{
  public: Identity ConstructSdfWorld(
      const Identity &/*_engine*/,
      const ::sdf::World &_sdfWorld) override;

  public: Identity ConstructSdfModel(
      const Identity &_worldID,
      const ::sdf::Model &_sdfModel) override;

  public: Identity ConstructSdfLink(
      const Identity &_modelID,
      const ::sdf::Link &_sdfLink) override;

  public: Identity ConstructSdfJoint(
      const Identity &_modelID,
      const ::sdf::Joint &_sdfJoint) override;

  public: Identity ConstructSdfCollision(
      const Identity &_linkID,
      const ::sdf::Collision &_collision) override;

  public: Identity ConstructSdfVisual(
      const Identity &_linkID,
      const ::sdf::Visual &_visual) override
      { return this->GenerateInvalidId(); };

  public: void FinalizeSdfModels(
    const Identity &_engine) override;
};

}
}
}

#endif
