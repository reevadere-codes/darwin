// Copyright 2018 The Darwin Neuroevolution Framework Authors.
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

#include "brain.h"
#include "cne.h"
#include "feedforward.h"
#include "genotype.h"

#include <core/utils.h>
#include <core/darwin.h>

#include <memory>
#include <vector>
using namespace std;

// A drastically simplified LSTM variation, where the inner cell value
// is the only recurrence, with a single threshold gate which controls
// the updates to this cell.

namespace cne {
namespace lstm_lite {

enum LstmLiteWeightIds { Wg, Ug, Bg, Wc, Nweights };

struct Gene : public feedforward::Gene {
  // LSTM_Lite weights: lw[OUTPUTS][Nweights]
  ann::Matrix lw;

  Gene() = default;
  Gene(size_t inputs, size_t outputs);

  void crossover(const Gene& parent1, const Gene& parent2, float preference);
  void mutate(float mutation_std_dev);
  void randomize();

  friend void to_json(json& json_obj, const Gene& gene);
  friend void from_json(const json& json_obj, Gene& gene);
};

struct Layer : public cne::AnnLayer {
  explicit Layer(const Gene& gene);

  vector<float> cells;

  // points directly to the weights in the genotype
  const ann::Matrix& w;
  const ann::Matrix& lw;

  void evaluate(const vector<float>& inputs) override;
  void resetState() override;
};

struct GenotypeTraits {
  using HiddenLayerGene = lstm_lite::Gene;
  using OutputLayerGene = feedforward::Gene;
};

using Genotype = cne::Genotype<GenotypeTraits>;

struct BrainTraits {
  using Genotype = lstm_lite::Genotype;
  using HiddenLayer = lstm_lite::Layer;
  using OutputLayer = feedforward::Layer;

  static constexpr bool kNormalizeHiddenLayers = false;
};

using Brain = cne::Brain<BrainTraits>;

}  // namespace lstm_lite
}  // namespace cne
