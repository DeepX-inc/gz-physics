/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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

#include <gtest/gtest.h>

#include <tuple>

#include <sdf/Root.hh>
#include <sdf/World.hh>

#include <test/Utils.hh>

#include <ignition/plugin/Loader.hh>

#include <ignition/physics/Joint.hh>
#include <ignition/physics/RequestEngine.hh>

#include <ignition/physics/sdf/ConstructJoint.hh>
#include <ignition/physics/sdf/ConstructLink.hh>
#include <ignition/physics/sdf/ConstructModel.hh>
#include <ignition/physics/sdf/ConstructWorld.hh>

#include "lib/src/Entity.hh"
#include "lib/src/World.hh"
#include "World.hh"

struct TestFeatureList : ignition::physics::FeatureList<
    ignition::physics::tpeplugin::RetrieveWorld,
    ignition::physics::sdf::ConstructSdfLink,
    ignition::physics::sdf::ConstructSdfModel,
    ignition::physics::sdf::ConstructSdfWorld
> { };

using World = ignition::physics::World3d<TestFeatureList>;
using WorldPtr = ignition::physics::World3dPtr<TestFeatureList>;

auto LoadEngine()
{
  ignition::plugin::Loader loader;
  loader.LoadLib(tpe_plugin_LIB);

  ignition::plugin::PluginPtr tpe_plugin =
      loader.Instantiate("ignition::physics::tpeplugin::Plugin");

  auto engine =
      ignition::physics::RequestEngine3d<TestFeatureList>::From(tpe_plugin);
  return engine;
}

World LoadWorld(const std::string &_world)
{
  auto engine = LoadEngine();
  EXPECT_NE(nullptr, engine);

  sdf::Root root;
  const sdf::Errors &errors = root.Load(_world);
  EXPECT_EQ(0u, errors.size());

  EXPECT_EQ(1u, root.WorldCount());
  const sdf::World *sdfWorld = root.WorldByIndex(0);
  EXPECT_NE(nullptr, sdfWorld);

  auto world = engine->ConstructWorld(*sdfWorld);
  EXPECT_NE(nullptr, world);

  return *world;
}

// Test that the tpe plugin loaded all the relevant information correctly.
TEST(SDFFeatures_TEST, CheckTpeData)
{
  World world = LoadWorld(TEST_WORLD_DIR"/test.world");
  auto tpeWorld = world.GetTpeLibWorld();
  ASSERT_NE(nullptr, tpeWorld);

  ASSERT_EQ(6u, tpeWorld->GetChildCount());

  // check model 01
  {
    ignition::physics::tpelib::Entity &model =
        tpeWorld->GetChildByName("ground_plane");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        model.GetId());
    EXPECT_EQ("ground_plane", model.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, model.GetPose());
    EXPECT_EQ(1u, model.GetChildCount());

    ignition::physics::tpelib::Entity &link = model.GetChildByName("link");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link.GetId());
    EXPECT_EQ("link", link.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, link.GetPose());
    EXPECT_EQ(1u, link.GetChildCount());

    ignition::physics::tpelib::Entity &collision =
        link.GetChildByName("collision");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        collision.GetId());
    EXPECT_EQ("collision", collision.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, collision.GetPose());
  }

  // check model 02
  {
    ignition::physics::tpelib::Entity &model =
        tpeWorld->GetChildByName("double_pendulum_with_base");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        model.GetId());
    EXPECT_EQ("double_pendulum_with_base", model.GetName());
    EXPECT_EQ(ignition::math::Pose3d(1, 0, 0, 0, 0, 0), model.GetPose());
    EXPECT_EQ(3u, model.GetChildCount());

    ignition::physics::tpelib::Entity &link = model.GetChildByName("base");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link.GetId());
    EXPECT_EQ("base", link.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, link.GetPose());
    EXPECT_EQ(2u, link.GetChildCount());

    ignition::physics::tpelib::Entity &collision =
        link.GetChildByName("col_plate_on_ground");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        collision.GetId());
    EXPECT_EQ("col_plate_on_ground", collision.GetName());
    EXPECT_EQ(ignition::math::Pose3d(0, 0, 0.01, 0, 0, 0), collision.GetPose());

    ignition::physics::tpelib::Entity &collision02 =
        link.GetChildByName("col_pole");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        collision.GetId());
    EXPECT_EQ("col_pole", collision02.GetName());
    EXPECT_EQ(ignition::math::Pose3d(-0.275, 0, 1.1, 0, 0, 0),
        collision02.GetPose());

    ignition::physics::tpelib::Entity &link02 =
        model.GetChildByName("upper_link");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link02.GetId());
    EXPECT_EQ("upper_link", link02.GetName());
    EXPECT_EQ(
        ignition::math::Pose3d(0, 0, 2.1, -1.5708, 0, 0),
        link02.GetPose());
    EXPECT_EQ(3u, link02.GetChildCount());

    ignition::physics::tpelib::Entity &collision03 =
        link02.GetChildByName("col_upper_joint");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        collision03.GetId());
    EXPECT_EQ("col_upper_joint", collision03.GetName());
    EXPECT_EQ(ignition::math::Pose3d(-0.05, 0, 0, 0, 1.5708, 0),
        collision03.GetPose());

    ignition::physics::tpelib::Entity &collision04 =
        link02.GetChildByName("col_lower_joint");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        collision04.GetId());
    EXPECT_EQ("col_lower_joint", collision04.GetName());
    EXPECT_EQ(ignition::math::Pose3d(0, 0, 1.0, 0, 1.5708, 0),
        collision04.GetPose());

    ignition::physics::tpelib::Entity &collision05 =
        link02.GetChildByName("col_cylinder");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        collision05.GetId());
    EXPECT_EQ("col_cylinder", collision05.GetName());
    EXPECT_EQ(ignition::math::Pose3d(0, 0, 0.5, 0, 0, 0),
        collision05.GetPose());

    ignition::physics::tpelib::Entity &link03 =
        model.GetChildByName("lower_link");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link03.GetId());
    EXPECT_EQ("lower_link", link03.GetName());
    EXPECT_EQ(ignition::math::Pose3d(0.25, 1.0, 2.1, -2, 0, 0),
        link03.GetPose());
    EXPECT_EQ(2u, link03.GetChildCount());

    ignition::physics::tpelib::Entity &collision06 =
        link03.GetChildByName("col_lower_joint");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        collision06.GetId());
    EXPECT_EQ("col_lower_joint", collision06.GetName());
    EXPECT_EQ(ignition::math::Pose3d(0, 0, 0, 0, 1.5708, 0),
        collision06.GetPose());

    ignition::physics::tpelib::Entity &collision07 =
        link03.GetChildByName("col_cylinder");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        collision07.GetId());
    EXPECT_EQ("col_cylinder", collision07.GetName());
    EXPECT_EQ(ignition::math::Pose3d(0, 0, 0.5, 0, 0, 0),
        collision07.GetPose());
  }

  // check model 03
  {
    ignition::physics::tpelib::Entity &model =
        tpeWorld->GetChildByName("free_body");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        model.GetId());
    EXPECT_EQ("free_body", model.GetName());
    EXPECT_EQ(ignition::math::Pose3d(0, 10, 10, 0, 0, 0), model.GetPose());
    EXPECT_EQ(1u, model.GetChildCount());

    ignition::physics::tpelib::Entity &link = model.GetChildByName("link");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link.GetId());
    EXPECT_EQ("link", link.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, link.GetPose());
    EXPECT_EQ(0u, link.GetChildCount());
  }

  // check model 04
  {
    ignition::physics::tpelib::Entity &model =
        tpeWorld->GetChildByName("joint_limit_test");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        model.GetId());
    EXPECT_EQ("joint_limit_test", model.GetName());
    EXPECT_EQ(ignition::math::Pose3d(10, 0, 2, 0, 0, 0), model.GetPose());
    EXPECT_EQ(2u, model.GetChildCount());

    ignition::physics::tpelib::Entity &link = model.GetChildByName("base");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link.GetId());
    EXPECT_EQ("base", link.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, link.GetPose());
    EXPECT_EQ(0u, link.GetChildCount());

    ignition::physics::tpelib::Entity &link02 = model.GetChildByName("bar");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link02.GetId());
    EXPECT_EQ("bar", link02.GetName());
    EXPECT_EQ(ignition::math::Pose3d(1, 0, 0, 0, 0, 0), link02.GetPose());
    EXPECT_EQ(0u, link02.GetChildCount());
  }

  // check model 05
  {
    ignition::physics::tpelib::Entity &model =
        tpeWorld->GetChildByName("screw_joint_test");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        model.GetId());
    EXPECT_EQ("screw_joint_test", model.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, model.GetPose());
    EXPECT_EQ(2u, model.GetChildCount());

    ignition::physics::tpelib::Entity &link = model.GetChildByName("link0");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link.GetId());
    EXPECT_EQ("link0", link.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, link.GetPose());
    EXPECT_EQ(0u, link.GetChildCount());

    ignition::physics::tpelib::Entity &link02 = model.GetChildByName("link1");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link02.GetId());
    EXPECT_EQ("link1", link02.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, link02.GetPose());
    EXPECT_EQ(0u, link02.GetChildCount());
  }

  // check model 06
  {
    ignition::physics::tpelib::Entity &model =
        tpeWorld->GetChildByName("unsupported_joint_test");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        model.GetId());
    EXPECT_EQ("unsupported_joint_test", model.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, model.GetPose());
    EXPECT_EQ(6u, model.GetChildCount());

    ignition::physics::tpelib::Entity &link = model.GetChildByName("link0");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link.GetId());
    EXPECT_EQ("link0", link.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, link.GetPose());
    EXPECT_EQ(0u, link.GetChildCount());

    ignition::physics::tpelib::Entity &link02 = model.GetChildByName("link1");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link02.GetId());
    EXPECT_EQ("link1", link02.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, link02.GetPose());
    EXPECT_EQ(0u, link02.GetChildCount());

    ignition::physics::tpelib::Entity &link03 = model.GetChildByName("link2");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link03.GetId());
    EXPECT_EQ("link2", link03.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, link03.GetPose());
    EXPECT_EQ(0u, link03.GetChildCount());

    ignition::physics::tpelib::Entity &link04 = model.GetChildByName("link3");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link04.GetId());
    EXPECT_EQ("link3", link04.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, link04.GetPose());
    EXPECT_EQ(0u, link04.GetChildCount());

    ignition::physics::tpelib::Entity &link05 = model.GetChildByName("link4");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link05.GetId());
    EXPECT_EQ("link4", link05.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, link05.GetPose());
    EXPECT_EQ(0u, link05.GetChildCount());

    ignition::physics::tpelib::Entity &link06 = model.GetChildByName("link5");
    ASSERT_NE(ignition::physics::tpelib::Entity::kNullEntity.GetId(),
        link06.GetId());
    EXPECT_EQ("link5", link06.GetName());
    EXPECT_EQ(ignition::math::Pose3d::Zero, link06.GetPose());
    EXPECT_EQ(0u, link06.GetChildCount());
  }
}

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}