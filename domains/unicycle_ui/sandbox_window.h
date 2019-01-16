// Copyright 2019 The Darwin Neuroevolution Framework Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <core_ui/box2d_sandbox_window.h>
#include <core_ui/properties_widget.h>
#include <domains/unicycle/agent.h>
#include <domains/unicycle/unicycle.h>
#include <domains/unicycle/world.h>

#include <QFrame>
#include <QIcon>
#include <QTimer>

#include <memory>
using namespace std;

namespace unicycle_ui {

class SandboxWindow : public core_ui::Box2dSandboxWindow {
  struct Variables {
    // configuration
    core_ui::PropertyItem* generation = nullptr;
    core_ui::PropertyItem* max_steps = nullptr;

    // simulation state
    core_ui::PropertyItem* state = nullptr;
    core_ui::PropertyItem* step = nullptr;
    core_ui::PropertyItem* pos = nullptr;
    core_ui::PropertyItem* velocity = nullptr;
    core_ui::PropertyItem* angle_1 = nullptr;
    core_ui::PropertyItem* angular_velocity_1 = nullptr;
    core_ui::PropertyItem* angle_2 = nullptr;
    core_ui::PropertyItem* angular_velocity_2 = nullptr;
  };

 public:
  bool setup();

  void newScene() override;
  void singleStep() override;
  void updateUI() override;

 private:
  void setupVariables();

 private:
  Variables variables_;

  const unicycle::Unicycle* domain_ = nullptr;

  shared_ptr<const darwin::Genotype> genotype_;
  unique_ptr<unicycle::World> world_;
  unique_ptr<unicycle::Agent> agent_;
  int step_ = -1;
  int max_steps_ = -1;
};

}  // namespace unicycle_ui