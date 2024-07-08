// Copyright 2024 The MediaPipe Authors.
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

#ifndef MEDIAPIPE_TASKS_TOOLS_MODEL_CKPT_UTIL_H_
#define MEDIAPIPE_TASKS_TOOLS_MODEL_CKPT_UTIL_H_

#include "odml/infra/genai/inference/utils/xnn_utils/model_ckpt_util.h"

#include "mediapipe/tasks/cc/text/utils/vocab_convert_utils.h"
#include "odml/infra/genai/inference/ml_drift/llm/tensor_loaders/model_ckpt_util.h"
#include "pybind11/pybind11.h"
#include "pybind11_abseil/status_casters.h"  // from @pybind11_abseil

PYBIND11_MODULE(model_ckpt_util, m) {
  pybind11::google::ImportStatusModule();
  m.doc() = "Pybind model checkpoint utility functions.";

  m.def("GenerateCpuTfLite", &odml::infra::xnn_utils::GenerateTfLite,
        "Generates the TfLite flatbuffer file from the serialized weight files "
        "for the CPU backend.");
  m.def("GenerateGpuTfLite", &odml::infra::gpu::GenerateTfLite,
        "Generates the TfLite flatbuffer file from the serialized weight files "
        "for the GPU backend.");
  m.def("ConvertHfTokenizer", &mediapipe::tasks::text::ConvertHfTokenizer,
        "Converts the HuggingeFace BPE tokenizer to internal SentencePiece "
        "vocab model.");
}

#endif  // MEDIAPIPE_TASKS_TOOLS_MODEL_CKPT_UTIL_H_
