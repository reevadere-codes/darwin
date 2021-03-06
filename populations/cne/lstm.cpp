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

#include "lstm.h"

#include <core/ann_dynamic.h>
#include <core/ann_utils.h>

#include <assert.h>

namespace cne {

template <>
unique_ptr<darwin::Brain> lstm::Genotype::grow() const {
  return make_unique<lstm::Brain>(this);
}

namespace lstm {

Gene::Gene(size_t inputs, size_t outputs)
    : feedforward::Gene(inputs, outputs), lw(outputs, Nweights) {}

void Gene::crossover(const Gene& parent1, const Gene& parent2, float preference) {
  feedforward::Gene::crossover(parent1, parent2, preference);
  crossoverOperator(lw, parent1.lw, parent2.lw, preference);
}

void Gene::mutate(float mutation_std_dev) {
  feedforward::Gene::mutate(mutation_std_dev);
  mutationOperator(lw, mutation_std_dev);
}

void Gene::randomize() {
  feedforward::Gene::randomize();
  ann::randomize(lw);
}

void to_json(nlohmann::json& json_obj, const Gene& gene) {
  to_json(json_obj, static_cast<const feedforward::Gene&>(gene));
  json_obj["lw"] = gene.lw;
}

void from_json(const json& json_obj, Gene& gene) {
  from_json(json_obj, static_cast<feedforward::Gene&>(gene));
  gene.lw = json_obj.at("lw");
  if (gene.lw.cols != Nweights || gene.lw.rows != gene.w.cols)
    throw core::Exception("Can't load gene, inconsistent LSTM weights");
}

Layer::Layer(const Gene& gene)
    : cne::AnnLayer(gene.w.cols), cells(gene.w.cols), w(gene.w), lw(gene.lw) {
  CHECK(lw.cols == Nweights);
}

void Layer::evaluate(const vector<float>& inputs) {
  assert(inputs.size() == w.rows - 1);
  assert(values.size() == lw.rows);
  assert(values.size() == w.cols);

  const size_t bias_index = w.rows - 1;
  for (size_t i = 0; i < w.cols; ++i) {
    float v = w[bias_index][i];
    for (size_t j = 0; j < bias_index; ++j)
      v += inputs[j] * w[j][i];

    const float prev = values[i];
    float cand_C = ann::activate(lw[i][Wc] * v + lw[i][Uc] * prev + lw[i][Bc]);
    float i_gate = ann::activateGate(lw[i][Wi] * v + lw[i][Ui] * prev + lw[i][Bi]);
    float f_gate = ann::activateGate(lw[i][Wf] * v + lw[i][Uf] * prev + lw[i][Bf]);
    float o_gate = ann::activateGate(lw[i][Wo] * v + lw[i][Uo] * prev + lw[i][Bo]);
    cells[i] = f_gate * cells[i] + i_gate * cand_C;
    values[i] = o_gate * ann::activate(cells[i]);
  }
}

void Layer::resetState() {
  ann::reset(values);
  ann::reset(cells);
}

}  // namespace lstm
}  // namespace cne
