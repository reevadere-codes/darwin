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

#include <core/properties.h>
#include <core/selection_algorithm.h>

#include <vector>
using namespace std;

namespace selection {

//! Truncation selection configuration
struct TruncationSelectionConfig : public core::PropertySet {
  PROPERTY(elite_percentage, float, 0.1f, "Elite percentage");
  PROPERTY(elite_min_fitness, float, 0.0f, "Elite minimum fitness");
  PROPERTY(elite_mutation_chance, float, 0.0f, "Elite mutation chance");
};

//! A truncation-selection variant
//! 
//! It support elitism. The parents for the next generation are randomly 
//! selected based on the raking order of the previous generation.
//!
class TruncationSelection : public selection::SelectionAlgorithm {
 public:
  explicit TruncationSelection(const core::PropertySet& config);

  void newPopulation(darwin::Population* population) override;
  void createNextGeneration(selection::GenerationFactory* next_generation) override;

 private:
  darwin::Population* population_ = nullptr;
  TruncationSelectionConfig config_;
};

}  // namespace selection
