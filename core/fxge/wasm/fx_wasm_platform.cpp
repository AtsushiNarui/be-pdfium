// Copyright 2025 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxge/cfx_gemodule.h"

#include <memory>

#include "core/fxge/cfx_folderfontinfo.h"

namespace {

class CWasmPlatform final : public CFX_GEModule::PlatformIface {
 public:
  CWasmPlatform() = default;
  ~CWasmPlatform() override = default;

  // CFX_GEModule::PlatformIface:
  void Init() override {}
  std::unique_ptr<SystemFontInfoIface> CreateDefaultSystemFontInfo() override {
    return std::make_unique<CFX_FolderFontInfo>();
  }
};

}  // namespace

std::unique_ptr<CFX_GEModule::PlatformIface>
CFX_GEModule::PlatformIface::Create() {
  return std::make_unique<CWasmPlatform>();
}
